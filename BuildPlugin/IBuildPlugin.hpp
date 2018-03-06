#ifndef BUILDPLUGIN_INTERFACE_H
#define BUILDPLUGIN_INTERFACE_H

#include <QtPlugin>
#include <QObject>

class IBuildPlugin
{

public:

    virtual ~IBuildPlugin() {}
    virtual QObject* getObject() = 0;
    virtual bool build(QJsonObject buildInfo) = 0;
    virtual void onInfoMessage(QString message) = 0;
    virtual void onErrorMessage(QString message) = 0;
};

Q_DECLARE_INTERFACE(IBuildPlugin, "IBuildPlugin")

#endif // BUILDPLUGIN_INTERFACE_H
