#include "UploadPlugin.hpp"

#include <QFile>
#include <QTextStream>
#include <QJsonArray>
#include <QTimer>

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
    emit readDataFromExternalFlash(0x01);
    return true;
}

bool
UploadPlugin::
downloadProject(QJsonObject project)
{
    emit onInfoMessage("downloading...");
    //emit writeDataToExternalFlash(0x00, 0xAA);
    return true;
}

void
UploadPlugin::
onDataWrittenToExternalFlash(quint32 addr, quint8 byte)
{
    qDebug(QString("Data %2 written to @%1")
           .arg(QString::number(addr,16))
           .arg(QString::number(byte,16))
           .toUtf8());

    /*QTimer* timer = new QTimer();
    QObject::connect(timer, &QTimer::timeout, [this](){
        emit readDataFromExternalFlash(0x00);
    });
    timer->start(1000);*/
}

void
UploadPlugin::
onDataReadFromExternalFlash(quint32 addr, quint8 byte)
{
    qDebug(QString("Data %2 read from @%1")
           .arg(QString::number(addr,16))
           .arg(QString::number(byte,16))
           .toUtf8());
}
