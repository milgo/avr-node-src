#ifndef DOWNLOADPLUGIN_INTERFACE_H
#define DOWNLOADPLUGIN_INTERFACE_H

#include <QtPlugin>
#include <QObject>

class IDownloadPlugin
{

public:

    virtual ~IDownloadPlugin() {}
    virtual QObject* getObject() = 0;
    virtual bool download(QJsonObject downloadInfo) = 0;
    virtual void onInfoMessage(QString message) = 0;
    virtual void onErrorMessage(QString message) = 0;
    virtual void onDownloadFinished() = 0;
    virtual void onDownloadFail() = 0;
};

Q_DECLARE_INTERFACE(IDownloadPlugin, "IDownloadPlugin")

#endif // DOWNLOADPLUGIN_INTERFACE_H
