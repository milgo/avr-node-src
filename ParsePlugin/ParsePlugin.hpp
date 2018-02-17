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

    QJsonObject parseFiles(QString path) override;
    void compile(QJsonObject program, QString path) override;

signals:

    void onErrorMessage(QString message) override;

private:

    QString compileNode(QJsonArray program, QJsonObject root);
};

#endif // PLUGIN_H
