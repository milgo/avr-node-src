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
    void sendRequestToDevice(quint8 command, quint8 param, quint32 data);

public slots:

    void onReplyFromDevice(quint16 id, quint8 command, quint8 param, quint32 data);

};

#endif // UPLOADPLUGIN_H
