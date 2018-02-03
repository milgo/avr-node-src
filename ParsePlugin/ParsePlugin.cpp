#include "ParsePlugin.hpp"

#include <QFile>
#include <QTextStream>
#include <QJsonArray>

ParsePlugin::
ParsePlugin()
{
}

QObject*
ParsePlugin::
getObject ()
{
    return this;
}

bool
ParsePlugin::
parseFiles()
{
    QString filePath;

    QFile file(filePath.toLatin1());
    if(!file.open(QIODevice::ReadOnly)) {
        //QMessageBox::information(0, "Error", file.errorString());
        return false;
    }

    QTextStream in(&file);

    QRegExp paramDefRegEx("\\w+ \\w+");
    while(!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if(line.startsWith("/*VARDEF*/"))
        {
            line = line.remove(0,11);
            QStringList split = line.split(" ");
            QJsonObject varJson;
            varJson["type"] = split[1];
            emit this->onParseNewVariableType(varJson);
        }
        else if(line.startsWith("/*"))
        {
          QRegExp sep("(\\/\\*)|(\\*\\/)");
          QStringList params = line.section(sep,1,1).split(",");
          QString categoryName = params[0];

          line = line.remove(0,line.indexOf("*\\")+2);
          line = line.trimmed();

          QJsonObject funcJson;
          QJsonArray funcArgsJson;

          int offset = 0;
          int counter = 0;

          while ( (offset = paramDefRegEx.indexIn(line, offset)) != -1 )
          {
              offset += paramDefRegEx.matchedLength();

              QStringList split = paramDefRegEx.cap(0).split(QRegExp("\\s"));

              if(counter == 0)
              {
                  funcJson["name"] = split[1].toUpper();;
                  funcJson["returnType"] = split[0].toUpper();
                  funcJson["category"] = categoryName;
              }
              else if(counter == 1)
              {

              }
              else
              {
                  QJsonObject arg;
                  arg["name"] = split[1].toUpper();
                  arg["type"] = split[0].toUpper();
                  arg["value"] = QString("");
                  funcArgsJson.append(arg);
              }

              counter++;
          }

          funcJson["args"] = funcArgsJson;

          if(funcJson["name"].toString().length()>0)
            emit this->onParseNewFunctionBlock(funcJson);
        }
    }

    QJsonObject constJSon;
    constJSon["name"] = QString("UCONST");
    constJSon["type"] = QString("UINT");
    constJSon["returnType"] = QString("UINT");
    constJSon["category"] = QString("CONSTS");

    emit onParseNewConstType(constJSon);

    file.close();

    return true;
}

void
ParsePlugin::
compile(QJsonObject programJson)
{
}

void
ParsePlugin::
build()
{

}

void
ParsePlugin::
program()
{

}

void
ParsePlugin::
download()
{

}

void
ParsePlugin::
upload()
{

}

void
ParsePlugin::
connect(QJsonObject connectionJson)
{

}
