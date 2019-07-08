#ifndef BUILDPLUGIN_INTERFACE_H
#define BUILDPLUGIN_INTERFACE_H

#include <QtPlugin>
#include <QObject>

class IBuildPlugin
{

public:

    virtual ~IBuildPlugin() {}
    virtual QObject* getObject() = 0;
    virtual bool build(QJsonObject config, QJsonObject deviceConfig) = 0;
    virtual bool verifyDeviceConfig(QJsonObject deviceConfig, QString version) = 0;
    virtual void onInfoMessage(QString message) = 0;
    virtual void onErrorMessage(QString message) = 0;
    virtual void onBuildSuccess() = 0;
    virtual void onBuildFail() = 0;
};

Q_DECLARE_INTERFACE(IBuildPlugin, "IBuildPlugin")

#endif // BUILDPLUGIN_INTERFACE_H
