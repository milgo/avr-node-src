#ifndef DOWNLOADPLUGIN_H
#define DOWNLOADPLUGIN_H

#include <QtPlugin>
#include <QObject>
#include <QProcess>

#include "DownloadPlugin_global.hpp"
#include "IDownloadPlugin.hpp"

class DOWNLOADPLUGINSHARED_EXPORT DownloadPlugin : public QObject, IDownloadPlugin
{

    Q_OBJECT
    Q_INTERFACES(IDownloadPlugin)
    Q_PLUGIN_METADATA(IID "com.downloadplugin")

public:

    DownloadPlugin();
    QObject* getObject() override;

    bool download(QJsonObject config, QJsonObject deviceConfig) override;

public slots:

    void readStdout();
    void readStderr();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processError(QProcess::ProcessError error);

signals:

    void onErrorMessage(QString message) override;
    void onInfoMessage(QString message) override;
    void onDownloadFinished() override;
    void onDownloadFail() override;

private:

    QProcess process;
    bool errors;

};

#endif // DOWNLOADPLUGIN_H
