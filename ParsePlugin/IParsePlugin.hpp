#ifndef PARSEPLUGIN_INTERFACE_H
#define PARSEPLUGIN_INTERFACE_H

#include <QtPlugin>
#include <QObject>

class IParsePlugin
{

public:

    virtual ~IParsePlugin() {}
    virtual QObject* getObject() = 0;
    virtual QJsonObject parseFiles(QString path) = 0;
    virtual bool compile(QJsonObject project,QJsonObject program, QString path) = 0;
    virtual void onInfoMessage(QString message) = 0;
    virtual void onErrorMessage(QString message) = 0;
    virtual void onParseSuccess() = 0;
    virtual void onParseFail() = 0;
    virtual void onCompileSuccess() = 0;
    virtual void onCompileFail() = 0;
};

Q_DECLARE_INTERFACE(IParsePlugin, "IParsePlugin")

#endif // PARSEPLUGIN_INTERFACE_H
