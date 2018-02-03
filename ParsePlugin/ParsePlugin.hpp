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

    bool parseFiles() override;
    void compile(QJsonObject programJson) override;
    void build() override;
    void program() override;
    void download() override;
    void upload() override;
    void connect(QJsonObject connectionJson) override;

signals:

    void onParseNewConstType(QJsonObject constJson);
    void onParseNewVariableType(QJsonObject varJson);
    void onParseNewFunctionBlock(QJsonObject funcJson);
};

#endif // PLUGIN_H
