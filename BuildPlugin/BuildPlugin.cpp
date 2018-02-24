#include "BuildPlugin.hpp"

#include <QFile>
#include <QTextStream>
#include <QJsonArray>
#include <QProcess>

BuildPlugin::
BuildPlugin()
{
}

QObject*
BuildPlugin::
getObject ()
{
    return this;
}

void
BuildPlugin::
build(QJsonObject buildInfo)
{
    QString workingDir = buildInfo["build_directory"].toString();
    QProcess process;

    process.setWorkingDirectory(workingDir);
    process.start("make");
    process.waitForFinished(-1);

    QString stdoutstr(process.readAllStandardOutput());
    QString stderrstr (process.readAllStandardError());

    QStringList stdoutlist = stdoutstr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    QStringList stderrlist = stderrstr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);

    for(int i=0;i<stdoutlist.length();i++){
        onInfoMessage(stdoutlist[i]);
    }

    for(int i=0;i<stderrlist.length();i++){
        onErrorMessage(stderrlist[i]);
    }

    consoleWidget->setCurrentRow(consoleWidget->count()-1);
}


