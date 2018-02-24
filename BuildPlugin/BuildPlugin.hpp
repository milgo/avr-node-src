#ifndef BUILDPLUGIN_H
#define BUILDPLUGIN_H

#include <QtPlugin>
#include <QObject>

#include "BuildPlugin_global.hpp"
#include "IBuildPlugin.hpp"

class BUILDPLUGINSHARED_EXPORT BuildPlugin: public QObject, IBuildPlugin
{

    Q_OBJECT
    Q_INTERFACES(IBuildPlugin)
    Q_PLUGIN_METADATA(IID "com.buildplugin")

public:

    BuildPlugin();
    QObject* getObject() override;

    void build(QJsonObject buildInfo) override;

signals:

    void onErrorMessage(QString message) override;
    void onInfoMessage(QString message) override;

};

#endif // BUILDPLUGIN_H
