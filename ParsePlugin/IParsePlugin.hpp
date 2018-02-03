#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

#include <QtPlugin>
#include <QObject>

class IParsePlugin
{

public:

    virtual ~IParsePlugin() {}
    virtual QObject* getObject() = 0;
    virtual bool parseFiles() = 0;
    virtual void compile(QJsonObject programJson) = 0;
    virtual void build() = 0;
    virtual void program() = 0;
    virtual void download() = 0;
    virtual void upload() = 0;
    virtual void connect(QJsonObject connectionJson) = 0;
    virtual void onParseNewConstType(QJsonObject constJson) = 0;
    virtual void onParseNewVariableType(QJsonObject varJson) = 0;
    virtual void onParseNewFunctionBlock(QJsonObject funcJson) = 0;

};

Q_DECLARE_INTERFACE(IParsePlugin, "IParsePlugin")

#endif // PLUGIN_INTERFACE_H
