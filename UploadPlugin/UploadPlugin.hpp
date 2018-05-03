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

    bool upload() override;

signals:

    void onErrorMessage(QString message) override;
    void onInfoMessage(QString message) override;
    void onUploadSuccess(QJsonObject uploadedProject) override;
    void onUploadFail() override;

};

#endif // UPLOADPLUGIN_H
