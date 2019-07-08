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

    bool build(QJsonObject config, QJsonObject deviceConfig) override;
    bool verifyDeviceConfig(QJsonObject config, QString version) override;

public slots:

    void readStdout();
    void readStderr();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processError(QProcess::ProcessError error);

signals:

    void onErrorMessage(QString message) override;
    void onInfoMessage(QString message) override;
    void onBuildSuccess() override;
    void onBuildFail() override;

private:

    QProcess process;
    bool errors;

};

#endif // BUILDPLUGIN_H
