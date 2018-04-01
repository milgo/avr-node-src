#include "DownloadPlugin.hpp"

#include <QFile>
#include <QTextStream>
#include <QJsonArray>

DownloadPlugin::
DownloadPlugin()
{
    connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStdout()));
    connect(&process, SIGNAL(readyReadStandardError()), this, SLOT(readStderr()));
    connect(&process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
    connect(&process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
    errors = false;
}

QObject*
DownloadPlugin::
getObject ()
{
    return this;
}

bool
DownloadPlugin::
download(QJsonObject downloadInfo)
{
    QString workingDir = downloadInfo["file_directory"].toString();
    //QString fileName = downloadInfo["file_name"].toString();
    //QString comPort = downloadInfo["com_port"].toString();
    process.setWorkingDirectory(workingDir);
    process.start(downloadInfo["download_command"].toString());
    //process.start("avrdude -c arduino -p m8 -P "+comPort.toLatin1()+" -b 115200 -U flash:w:\""+fileName.toLatin1()+"\":a");
    errors = false;
    return true;
}



void
DownloadPlugin::
readStdout()
{
    QString stdoutstr(process.readAllStandardOutput());
    QStringList stdoutlist = stdoutstr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    for(int i=0;i<stdoutlist.length();i++){
        onInfoMessage(stdoutlist[i]);
    }
}


void
DownloadPlugin::
readStderr()
{
    QString stderrstr (process.readAllStandardError());
    QStringList stderrlist = stderrstr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    for(int i=0;i<stderrlist.length();i++){
        onInfoMessage(stderrlist[i]);
        //errors = true;
    }
}


void
DownloadPlugin::
processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    switch ( exitStatus )
    {
    case QProcess::NormalExit:
        if(errors)
        {
            emit onErrorMessage(tr( "Process finished with errors." ) );
            //emit onDownloadFail();
        }
        else
        {
            emit onInfoMessage(tr( "Process finished without errors." ) );
            //emit onDownloadSuccess();
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
            //emit onDownloadFail();
        }
        else
        {
            emit onErrorMessage( tr( "Process crashed. Exit code: %1" ).arg( exitCode ) );
            //emit onDownloadFail();
        }
        break;
    }

    //process.deleteLater();
}


void
DownloadPlugin::
processError(QProcess::ProcessError error)
{
    QString msg;

    switch ( error )
    {
    case QProcess::FailedToStart:
        msg = tr( "Process failed to start." );
        break;

    case QProcess::Crashed: // Already reported via processFinished()
        break;

    case QProcess::Timedout:
        msg = tr( "Process timed out." );
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


