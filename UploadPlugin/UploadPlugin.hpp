#ifndef UPLOADPLUGIN_H
#define UPLOADPLUGIN_H

#include <QtPlugin>
#include <QObject>
#include <QProcess>
#include <QTimer>
#include <memory>

#include "UploadPlugin_global.hpp"
#include "IUploadPlugin.hpp"

#define EXTERNAL_FLASH_MAX_ADRESS 0x7FFFFF

class UPLOADPLUGINSHARED_EXPORT UploadPlugin : public QObject, IUploadPlugin
{

    Q_OBJECT
    Q_INTERFACES(IUploadPlugin)
    Q_PLUGIN_METADATA(IID "com.uploadplugin")

public:

    UploadPlugin();
    QObject* getObject() override;

    bool uploadProject() override;
    bool downloadProject(QJsonObject project) override;
    void uploadData(quint32 address, quint32 length);

signals:

    void onErrorMessage(QString message) override;
    void onInfoMessage(QString message) override;
    void onDownloadProjectSuccess() override;
    void onDownloadProjectFail() override;
    void onUploadProjectSuccess(QJsonObject uploadedProject) override;
    void onUploadProjectFail() override;

    void writeDataToExternalFlash(quint32 addr, quint8 data);
    void readDataFromExternalFlash(quint32 addr);

    void onUploadDataComplete(QByteArray data);

public slots:

    void onDataWrittenToExternalFlash(quint32 addr, quint8 byte);
    void onDataReadFromExternalFlash(quint32 addr, quint8 byte);

private:

    QByteArray dataToDownload;
    QByteArray uploadDataBuf;
    quint32 downloadIndex = 0;
    quint32 uploadSize = 0;
    quint32 uploadIndex = 0;

    std::shared_ptr<QMetaObject::Connection> uploadCompleteLambda;
};

#endif // UPLOADPLUGIN_H
