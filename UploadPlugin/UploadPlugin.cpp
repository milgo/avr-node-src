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
    uploadCompleteLambda = std::make_shared<QMetaObject::Connection>();
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
    //uploadData(0x7F0000, 4);
    uploadData(0x00, 4);

    *uploadCompleteLambda = QObject::connect(this, &UploadPlugin::onUploadDataComplete, [&](QByteArray data)
    {
        quint32 dataSize = data.at(3) << 24 | data.at(2) << 16 | data.at(1) << 8 | data.at(0);
        qDebug(QString::number(dataSize,16).toUtf8());
        //uploadData(0x7F0004, dataSize);
        uploadData(0x04, dataSize);
        QObject::disconnect(*uploadCompleteLambda);
        *uploadCompleteLambda = QObject::connect(this, &UploadPlugin::onUploadDataComplete, [&](QByteArray data){
            QByteArray uncompressedData = qUncompress(data);
            emit onUploadProjectSuccess(QJsonDocument::fromJson(uncompressedData).object());
            QObject::disconnect(*uploadCompleteLambda);
        });
    });

    return true;
}



bool
UploadPlugin::
downloadProject(QJsonObject project)
{
    emit onInfoMessage("downloading...");
    //emit writeDataToExternalFlash(0x7F0000, 0xaa);
    QJsonDocument doc(project);
    QByteArray compressedData = qCompress(doc.toJson(), 9);

    QByteArray compressedDataSize;
    quint32 dataSize = compressedData.size();
    for(int i = 0; i != sizeof(dataSize); ++i){
        compressedDataSize.append((char)((dataSize & (0xFF << (i*8))) >> (i*8)));
    }

    QDataStream dataStream(&dataToDownload, QIODevice::WriteOnly);
    dataStream.writeRawData(compressedDataSize.data(), compressedDataSize.size());
    dataStream.writeRawData(compressedData.data(), compressedData.size());    

    qDebug(dataToDownload.toHex());

    qDebug(QString("Before %1, after %2")
           .arg(QString::number(doc.toJson().size()))
           .arg(QString::number(dataToDownload.size()))
           .toUtf8());
    downloadIndex = 0;
    emit writeDataToExternalFlash(0x7F0000, dataToDownload.at(0));
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
    timer->setSingleShot(true);
        QObject::connect(timer, &QTimer::timeout, [this](){
            emit readDataFromExternalFlash(0x7F0000);
            emit onDownloadProjectSuccess();//just to disconnect remove later
        });
    timer->start(1000);*/

    if(addr - 0x7F0000 < dataToDownload.size())
    {
        emit writeDataToExternalFlash(++addr, dataToDownload.at(++downloadIndex));
    }
    else if(addr - 0x7F0000 == dataToDownload.size())
    {
        emit onDownloadProjectSuccess();
    }
    else
    {
        emit onDownloadProjectFail();
    }
}

void
UploadPlugin::
onDataReadFromExternalFlash(quint32 addr, quint8 byte)
{
    qDebug(QString("Data %2 read from %1")
           .arg(QString::number(addr,16))
           .arg(QString::number(byte,16))
           .toUtf8());

    ++uploadIndex;
    uploadDataBuf.append(byte);

    if(uploadIndex>=uploadSize)
    {
        emit onUploadDataComplete(uploadDataBuf);
    }
    else
    {
        emit readDataFromExternalFlash(++addr);
    }
}

void
UploadPlugin::
uploadData(quint32 address, quint32 length)
{
    uploadDataBuf.clear();
    uploadSize = length;
    uploadIndex = 0;

    readDataFromExternalFlash(address);

    /*if(address >= 0x7F0000)
    {
        emit readDataFromExternalFlash(address);
    }
    else if(address < 0x7F0000)
    {
        emit onUploadProjectFail();
    }*/
}
