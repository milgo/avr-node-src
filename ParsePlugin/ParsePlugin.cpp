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

QJsonObject
ParsePlugin::
parseFiles(QString path)
{
    QJsonObject resultJson;
    QJsonArray varTypes;
    QJsonArray constTypes;
    QJsonArray functions;

    QFile file(path.toLatin1());
    if(!file.open(QIODevice::ReadOnly)) {
        emit this->onErrorMessage(QString("ParsePlugin: Could not find files to parse!"));
        return resultJson;
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
            //emit this->onParseNewVariableType(varJson);
            varTypes.append(varJson);
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
              functions.append(funcJson);
            //emit this->onParseNewFunctionBlock(funcJson);
        }
    }

    QJsonObject constJson;
    constJson["name"] = QString("UCONST");
    constJson["type"] = QString("UINT");
    constJson["returnType"] = QString("UINT");
    constJson["category"] = QString("CONSTS");

    //emit onParseNewConstType(constJSon);
    constTypes.append(constJson);

    resultJson["variable_types"] = varTypes;
    resultJson["const_types"] = constTypes;
    resultJson["functions"] = functions;

    file.close();

    return resultJson;
}

void
ParsePlugin::
compile(QJsonObject program, QString path)
{
    QFile hfile( path + "/variables.h" );
    if ( hfile.open(QIODevice::WriteOnly) )
    {
        QTextStream stream( &hfile );
        stream << "#ifndef _VARIABLES_H_\r\n";
        stream << "#define _VARIABLES_H_\r\n\r\n";
        stream << "#include \"functions.h\"\r\n";
        stream << "#include \"max.h\"\r\n\r\n";

        QJsonArray variables = program["variables"].toArray();

        foreach (const QJsonValue & value, variables)
        {
            QJsonObject obj = value.toObject();
            stream << "#define " << obj["name"].toString() << " " << obj["id"].toString() << "\r\n";
        }

        stream << "\r\n" << endl;

        //declare variable arrays, setters and getters
        QJsonArray types = program["types"].toArray();

        foreach (const QJsonValue & value, types)
        {
            QString typeName = value.toString();
            stream << typeName << " set" << typeName << "(uint32_t index, " << typeName << " value);\r\n";
            stream << typeName << " get" << typeName << "(uint32_t index);" << "\r\n\r\n";
        }

        stream << "#endif" << endl;
    }

    //generate max definition file
    QFile mfile( path + "/max.h" );
    if ( mfile.open(QIODevice::WriteOnly) )
    {
        QTextStream stream( &mfile );
        stream << "#ifndef _MAX_H_" << "\r\n";
        stream << "#define _MAX_H_" << "\r\n\r\n";
        stream << "#define MAX_DATA" << " " << program["data_size"].toString() << "\r\n";
        stream << "\r\n#endif" << "\r\n";
    }

    QFile cfile( path + "/variables.c" );
    if ( cfile.open(QIODevice::WriteOnly) )
    {
        QTextStream stream( &cfile );
        stream << "#include \"variables.h\"\r\n\r\n";

        QJsonArray types = program["types"].toArray();
        foreach (const QJsonValue & value, types)
        {
            QString typeName = value.toString();
            stream << typeName << " set" << typeName << "(uint32_t index, " << typeName << " value){_data[index]=value; return value;}\r\n";
            stream << typeName << " get" << typeName << "(uint32_t index){return _data[index];}\r\n";
        }
    }

    QFile pfile( path + "/program.c" );
    if( pfile.open(QIODevice::WriteOnly))
    {
        QTextStream stream( &pfile );
        stream << "#include \"program.h\"\r\n";
        stream << "#include \"variables.h\"\r\n\r\n";
        stream << "void loop()\r\n{\r\n\tbegin();\r\n";

        QJsonArray nodes = program["program"].toArray();
        QJsonArray variables = program["variables"].toArray();

        foreach (const QJsonValue & value, nodes)
        {
            QJsonObject obj = value.toObject();

            if(!obj["root"].toString().compare("1"))
            {
                stream << "\t" + compileNode(nodes, variables, obj).toLatin1() + ";\r\n";
            }
        }

        stream << "\tend();\r\n}\r\n";
    }
}

QString
ParsePlugin::
compileNode(QJsonArray program, QJsonArray variables, QJsonObject root)
{
    QJsonArray args = root["args"].toArray();
    int argsSize = args.count();

    QString result;
    if(!root["type"].toString().compare("VARIABLE"))
    {
        QString varType;
        foreach (const QJsonValue & var, variables)
        {
            if(!var["name"].toString().compare(root["name"].toString())){
                varType = var["type"].toString();
                break;
            }
        }

        if(argsSize>0){
            result = QString("set"+varType+"("+root["id"].toString());
        }
        else{
            result = QString("get"+varType+"("+root["id"].toString());
        }
    }
    else if(!root["type"].toString().compare("FUNCTION"))
    {
        result = QString(root["name"].toString()+"("+root["id"].toString());
    }

    int argCounter = 0;
    if(argsSize > 0)result.append(",");
    foreach (const QJsonValue & arg, args)
    {
        QString param = arg.toString();
        QStringList paramSplit = param.split(":");

        if(!paramSplit.at(0).compare("CONST"))
        {
            result.append(paramSplit.at(1));
        }
        else
        {
            foreach(const QJsonValue & node, program)
            {
                QJsonObject obj = node.toObject();
                if(!obj["type"].toString().compare(paramSplit.at(0))&&
                        !obj["id"].toString().compare(paramSplit.at(1)))
                {
                    result.append(compileNode(program, variables, obj));
                }
            }
        }
        argCounter++;
        if(argCounter<argsSize)
            result.append(",");
    }

    result.append(")");
    return result;
}
