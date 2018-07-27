#ifndef MONITORPLUGIN_INTERFACE_H
#define MONITORPLUGIN_INTERFACE_H

#include <QtPlugin>
#include <QObject>

class IMonitorPlugin
{

public:

    virtual ~IMonitorPlugin() {}
    virtual QObject* getObject() = 0;
    virtual bool connectToDevice(QJsonObject connectionInfo) = 0;
    virtual void onConnectionFailed();
    virtual bool disconnectFromDevice() = 0;
    virtual bool isConnected() = 0;

    virtual void onInfoMessage(QString message) = 0;
    virtual void onErrorMessage(QString message) = 0;
};

Q_DECLARE_INTERFACE(IMonitorPlugin, "IMonitorPlugin")

#endif // MONITORPLUGIN_INTERFACE_H
