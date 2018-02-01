#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

#include <QtPlugin>
#include <QObject>

class IParsePlugin
{

public:
    virtual ~IParsePlugin() {}
    virtual QObject* getObject() = 0;
    virtual bool load() = 0;
    virtual void compile(QString jsonProgram) = 0;
    virtual void onParseNewVariable(QString varType) = 0;
    virtual void onParseNewFunctionBlock(QString funcName,
                                         QString funcType,
                                         QString jsonArgs) = 0;
};

Q_DECLARE_INTERFACE(IParsePlugin, "IParsePlugin")

#endif // PLUGIN_INTERFACE_H
