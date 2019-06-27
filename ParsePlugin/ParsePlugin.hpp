#ifndef PARSEPLUGIN_H
#define PARSEPLUGIN_H

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

    QJsonObject parseFiles(QString path) override;
    bool compile(QJsonObject project, QJsonObject program, QString path) override;

signals:

    void onErrorMessage(QString message) override;
    void onInfoMessage(QString message) override;
    void onParseSuccess() override;
    void onParseFail() override;
    void onCompileSuccess() override;
    void onCompileFail() override;

private:

    QString compileNode(QJsonArray program, QJsonArray variables, QJsonObject root);
};

#endif // PARSEPLUGIN_H
