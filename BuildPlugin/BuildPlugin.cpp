#include "BuildPlugin.hpp"

#include <QFile>
#include <QTextStream>
#include <QJsonArray>
#include <QCoreApplication>
#include <QDir>

BuildPlugin::
BuildPlugin()
{
    connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStdout()));
    connect(&process, SIGNAL(readyReadStandardError()), this, SLOT(readStderr()));
    connect(&process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
    connect(&process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
    errors = false;
}

QObject*
BuildPlugin::
getObject ()
{
    return this;
}

bool
BuildPlugin::
build(QJsonObject config, QJsonObject deviceConfig)
{
    QString workingDir = QCoreApplication::applicationDirPath() + "/" +  config["build_dir"].toString();
    QString buildCommand = config["build_command"].toString() + " BOARD_DIR=" + config["board_dir"].toString() +
            " MCU=" + deviceConfig["mcu"].toString() + " F_CPU=" + deviceConfig["fcpu"].toString();
    process.setWorkingDirectory(workingDir);
    onInfoMessage(workingDir);
    onInfoMessage(buildCommand);
    process.start(buildCommand);
    errors = false;
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
        errors = true;
    }
}


void BuildPlugin::processFinished( int exitCode, QProcess::ExitStatus exitStatus )
{
    switch ( exitStatus )
    {
    case QProcess::NormalExit:
        if(errors)
        {
            emit onErrorMessage(tr( "Process finished with errors." ) );
            emit onBuildFail();
        }
        else
        {
            emit onInfoMessage(tr( "Process finished without errors." ) );
            emit onBuildSuccess();
        }
        break;

    case QProcess::CrashExit:

        if ( exitCode == 0 )
        {
            // Don't report an exit code of 0: Since we are starting all
            // processes with a shell, that exit code would be the exit
            // code of the shell; that would only be useful if the shell
            // crashed or could not be started.

            emit onErrorMessage( tr( "Process crashed." ) );
            emit onBuildFail();
        }
        else
        {
            emit onErrorMessage( tr( "Process crashed. Exit code: %1" ).arg( exitCode ) );
            emit onBuildFail();
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


