#ifndef BUILDPLUGIN_H
#define BUILDPLUGIN_H

#include <QtPlugin>
#include <QObject>
#include <QProcess>

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

public slots:

    void readStdout();
    void readStderr();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processError(QProcess::ProcessError error);

signals:

    void onErrorMessage(QString message) override;
    void onInfoMessage(QString message) override;

private:

    QProcess process;

};

#endif // BUILDPLUGIN_H
