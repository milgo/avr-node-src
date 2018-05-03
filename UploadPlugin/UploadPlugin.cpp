#include "UploadPlugin.hpp"

#include <QFile>
#include <QTextStream>
#include <QJsonArray>

UploadPlugin::
UploadPlugin()
{

}

QObject*
UploadPlugin::
getObject ()
{
    return this;
}

bool
UploadPlugin::
uploadProject()
{
    emit onInfoMessage("uploading...");
    return true;
}

bool
UploadPlugin::
downloadProject(QJsonObject project)
{
    emit onInfoMessage("downloading...");
    return true;
}

void
UploadPlugin::
onReplyFromDevice(quint16 id, quint8 command, quint8 param, quint32 data)
{

}
