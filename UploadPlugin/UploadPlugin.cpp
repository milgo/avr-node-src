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
upload()
{

    return true;
}


