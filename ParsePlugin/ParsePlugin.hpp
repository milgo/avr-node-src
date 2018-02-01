#ifndef PLUGIN_H
#define PLUGIN_H

#include <QtPlugin>
#include <QObject>

#include "ParsePlugin_global.hpp"
#include "IParsePlugin.hpp"

class PARSEPLUGINSHARED_EXPORT ParsePlugin: public QObject, IParsePlugin
{

    Q_OBJECT
    Q_INTERFACES(IParsePlugin)
    Q_PLUGIN_METADATA(IID "com.parseplugin")

public:

    ParsePlugin();
    QObject* getObject() override;
    bool load() override;
    void compile(QString jsonProgram) override;

signals:

    void onParseNewVariable(QString varType);
    void onParseNewFunctionBlock(QString funcName,
                                 QString funcType,
                                 QString jsonArgs);
};

#endif // PLUGIN_H
