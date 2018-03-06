#include "BuildPlugin.hpp"

#include <QFile>
#include <QTextStream>
#include <QJsonArray>

BuildPlugin::
BuildPlugin()
{
    connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStdout()));
    connect(&process, SIGNAL(readyReadStandardError()), this, SLOT(readStderr()));
    connect(&process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
    connect(&process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
}

QObject*
BuildPlugin::
getObject ()
{
    return this;
}

bool
BuildPlugin::
build(QJsonObject buildInfo)
{
    QString workingDir = buildInfo["build_directory"].toString();
    process.setWorkingDirectory(workingDir);
    process.start("make");
    return true;
}

void BuildPlugin::readStdout()
{
    QString stdoutstr(process.readAllStandardOutput());
    QStringList stdoutlist = stdoutstr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    for(int i=0;i<stdoutlist.length();i++){
        onInfoMessage(stdoutlist[i]);
    }
}


void BuildPlugin::readStderr()
{
    QString stderrstr (process.readAllStandardError());
    QStringList stderrlist = stderrstr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    for(int i=0;i<stderrlist.length();i++){
        onErrorMessage(stderrlist[i]);
    }
}


void BuildPlugin::processFinished( int exitCode, QProcess::ExitStatus exitStatus )
{
    switch ( exitStatus )
    {
    case QProcess::NormalExit:
        emit onInfoMessage(tr( "Process finished without errors." ) );
        break;

    case QProcess::CrashExit:

        if ( exitCode == 0 )
        {
        // Don't report an exit code of 0: Since we are starting all
        // processes with a shell, that exit code would be the exit
        // code of the shell; that would only be useful if the shell
        // crashed or could not be started.

        emit onErrorMessage( tr( "Process crashed." ) );
        }
        else
        {
        emit onErrorMessage( tr( "Process crashed. Exit code: %1" ).arg( exitCode ) );
        }
        break;
    }

    //process.deleteLater();
}


void BuildPlugin::processError( QProcess::ProcessError error )
{
    QString msg;

    switch ( error )
    {
    case QProcess::FailedToStart:
        msg = tr( "Error: Process failed to start." );
        break;

    case QProcess::Crashed: // Already reported via processFinished()
        break;

    case QProcess::Timedout:
        msg = tr( "Error: Process timed out." );
        break;

    case QProcess::ReadError:
        msg = tr( "Error reading output from the process." );
        break;

    case QProcess::WriteError:
        msg = tr( "Error writing data to the process." );
        break;

    case QProcess::UnknownError:
        msg = tr( "Unknown error." );
        break;
    }

    emit onErrorMessage(msg);
}


