#ifndef UPLOADPLUGIN_INTERFACE_H
#define UPLOADPLUGIN_INTERFACE_H

#include <QtPlugin>
#include <QObject>

class IUploadPlugin
{

public:

    virtual ~IUploadPlugin() {}
    virtual QObject* getObject() = 0;
    virtual bool upload() = 0;
    virtual void onInfoMessage(QString message) = 0;
    virtual void onErrorMessage(QString message) = 0;
    virtual void onUploadSuccess(QJsonObject uploadedProject) = 0;
    virtual void onUploadFail() = 0;
};

Q_DECLARE_INTERFACE(IUploadPlugin, "IUploadPlugin")

#endif // UPLOADPLUGIN_INTERFACE_H
