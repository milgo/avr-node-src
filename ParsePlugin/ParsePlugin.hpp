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

    bool parseFiles(QString path) override;
    void compile(QJsonObject program, QString path) override;
    void build(QString path) override;
    void program() override;
    void download() override;
    void upload() override;
    void connect(QJsonObject connection) override;

signals:

    void onParseNewConstType(QJsonObject constJson);
    void onParseNewVariableType(QJsonObject varJson);
    void onParseNewFunctionBlock(QJsonObject funcJson);
    void onErrorMessage(QString message);

private:

    QString compileNode(QJsonArray program, QJsonObject root);
};

#endif // PLUGIN_H
