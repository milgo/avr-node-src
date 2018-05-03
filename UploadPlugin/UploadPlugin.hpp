#ifndef UPLOADPLUGIN_H
#define UPLOADPLUGIN_H

#include <QtPlugin>
#include <QObject>
#include <QProcess>

#include "UploadPlugin_global.hpp"
#include "IUploadPlugin.hpp"

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

signals:

    void onErrorMessage(QString message) override;
    void onInfoMessage(QString message) override;
    void onDownloadProjectSuccess() override;
    void onDownloadProjectFail() override;
    void onUploadProjectSuccess(QJsonObject uploadedProject) override;
    void onUploadProjectFail() override;

    void writeDataToExternalFlash(quint32 addr, quint8 data);
    void readDataFromExternalFlash(quint32 addr);

public slots:

    void onDataWrittenToExternalFlash(quint32 addr, quint8 byte);
    void onDataReadFromExternalFlash(quint32 addr, quint8 byte);

};

#endif // UPLOADPLUGIN_H
