#include "UploadPlugin.hpp"

#include <QFile>
#include <QTextStream>
#include <QJsonArray>
#include <QTimer>
#include <QJsonDocument>
#include <QDataStream>
#include <QThread>

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

    //Read project size from external flash
    uploadData(EXTERNAL_FLASH_MAX_ADRESS - 637, 637);

    return true;
}

bool
UploadPlugin::
downloadProject(QJsonObject project)
{
    emit onInfoMessage("downloading...");
    emit writeDataToExternalFlash(0xFF, 0xAA);
    /*QJsonDocument doc(project);
    QByteArray compressedData = qCompress(doc.toJson(), 9);

    QByteArray compressedDataSize;
    quint32 dataSize = compressedData.size();
    for(int i = 0; i != sizeof(dataSize); ++i){
        compressedDataSize.append((char)((dataSize & (0xFF << (i*8))) >> (i*8)));
    }

    QDataStream dataStream(&dataToDownload, QIODevice::WriteOnly);
    dataStream.writeRawData(compressedData.data(), compressedData.size());
    dataStream.writeRawData(compressedDataSize.data(), compressedDataSize.size());

    qDebug(dataToDownload.toHex());

    qDebug(QString("Before %1, after %2")
           .arg(QString::number(doc.toJson().size()))
           .arg(QString::number(dataToDownload.size()))
           .toUtf8());
    downloadIndex = 0;
    emit writeDataToExternalFlash(EXTERNAL_FLASH_MAX_ADRESS - dataToDownload.size(), dataToDownload.at(0));
    return true;*/
}

void
UploadPlugin::
onDataWrittenToExternalFlash(quint32 addr, quint8 byte)
{

    qDebug(QString("Data %2 written to @%1")
           .arg(QString::number(addr,16))
           .arg(QString::number(byte,16))
           .toUtf8());

    QTimer* timer = new QTimer();
    timer->setSingleShot(true);
        QObject::connect(timer, &QTimer::timeout, [this](){
            emit readDataFromExternalFlash(0xFF);
            emit onDownloadProjectSuccess();//just to disconnect remove later
        });
    timer->start(1000);

    /*if(addr < EXTERNAL_FLASH_MAX_ADRESS)
    {
        emit writeDataToExternalFlash(++addr, dataToDownload.at(++downloadIndex));
    }
    else if(addr == EXTERNAL_FLASH_MAX_ADRESS)
    {
        emit onDownloadProjectSuccess();
    }
    else
    {
        emit onDownloadProjectFail();
    }*/
}

void
UploadPlugin::
onDataReadFromExternalFlash(quint32 addr, quint8 byte)
{
    qDebug(QString("Data %2 read from @%1")
           .arg(QString::number(addr,16))
           .arg(QString::number(byte,16))
           .toUtf8());

    /*++uploadIndex;
    uploadDataBuf.append(byte);

    if(uploadIndex>=uploadSize)
    {
        emit onUploadDataComplete(uploadDataBuf);
    }
    else
    {
        emit readDataFromExternalFlash(++addr);
    }*/
}

void
UploadPlugin::
uploadData(quint32 address, quint32 length)
{
    uploadDataBuf.clear();
    uploadSize = length;
    uploadIndex = 0;
    if(address < EXTERNAL_FLASH_MAX_ADRESS)
    {
        emit readDataFromExternalFlash(++address);
    }
    else if(address > EXTERNAL_FLASH_MAX_ADRESS)
    {
        emit onUploadProjectFail();
    }
}
