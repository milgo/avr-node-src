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
    virtual bool compile(QJsonObject program, QString path) = 0;
    virtual void onErrorMessage(QString message) = 0;
};

Q_DECLARE_INTERFACE(IParsePlugin, "IParsePlugin")

#endif // PARSEPLUGIN_INTERFACE_H
