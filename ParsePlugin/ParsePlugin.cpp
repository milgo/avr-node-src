#include "ParsePlugin.hpp"

#include <QFile>
#include <QTextStream>
#include <QJsonArray>
#include <QDirIterator>

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
        emit this->onParseFail();
    }
    else
    {
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
                      funcJson["isBoolType"] = QString("0");
                      funcJson["isVoidType"] = QString("0");
                      if(!funcJson["returnType"].toString().compare("BOOL"))
                          funcJson["isBoolType"] = QString("1");
                      if(!funcJson["returnType"].toString().compare("VOID"))
                          funcJson["isVoidType"] = QString("1");
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

        emit onParseSuccess();
    }

    return resultJson;
}

bool
ParsePlugin::
compile(QJsonObject program, QString path)
{
    bool errors = false;
    QJsonArray variables = program["variables"].toArray();
    QJsonArray types = program["types"].toArray();

    /* VARIABLES DEFINITION STRING */
    QString varDefineString;
    QTextStream varDefineStream( &varDefineString );
    foreach (const QJsonValue & value, variables)
    {
        QJsonObject obj = value.toObject();
        varDefineStream << "#define " << obj["name"].toString() << " " << obj["id"].toString() << endl;
    }

    varDefineStream << endl << endl;

    foreach (const QJsonValue & value, types)
    {
        QString typeName = value.toString();
        varDefineStream << typeName << " set" << typeName << "(uint32_t index, " << typeName << " value);" << endl;
        varDefineStream << typeName << " get" << typeName << "(uint32_t index);" << endl << endl;
    }

    /* CHECKSUM DEFINITION STRING */
    QString checksumDefineString;
    QTextStream checksumDefineStream( &checksumDefineString );
    checksumDefineStream << "#define CHECKSUM 0x" << program["crc32"].toString() << endl;

    /* MAX DATA SIZE DEFINITION STRING */
    QString maxDataDefineString;
    QTextStream maxDataDefineStream( &maxDataDefineString );
    maxDataDefineStream << "#define MAX_DATA" << " " << program["data_size"].toString() << endl;

    /* VARIABLES DECLARATION STRING */
    QString varDeclString;
    QTextStream varDeclStream( &varDeclString );

    //QJsonArray types = program["types"].toArray();
    foreach (const QJsonValue & value, types)
    {
        QString typeName = value.toString();
        varDeclStream << typeName << " set" << typeName << "(uint32_t index, " << typeName << " value){_data[index]=value; return value;}" << endl;
        varDeclStream << typeName << " get" << typeName << "(uint32_t index){return _data[index];}" << endl;
    }

    /* MAIN PROGRAM STRING */
    QString programString;
    QTextStream programStream(&programString);

    QJsonArray nodes = program["program"].toArray();
    //QJsonArray variables = program["variables"].toArray();

    foreach (const QJsonValue & value, nodes)
    {
        QJsonObject obj = value.toObject();

        if(!obj["root"].toString().compare("1"))
        {
            programStream << "\t" + compileNode(nodes, variables, obj).toLatin1() << endl;
        }
    }

    /* FIND TEMPLATES AND INSERT STRINGS */
    QDirIterator it(path, QStringList() << "*.*_", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString sourceFilePath = it.next();
        QString descFilePath = sourceFilePath.left(sourceFilePath.lastIndexOf(QChar('_')));

        QFile sourceFile(sourceFilePath);
        QFile descFile(descFilePath);

        if ( sourceFile.open(QIODevice::ReadOnly) )
        {
            if ( descFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
            {
                QTextStream instream(&sourceFile);
                QTextStream outstream(&descFile);
                while (!instream.atEnd())
                {
                    QString line = instream.readLine();

                    if(line.trimmed() == "%MAIN_PROGRAM%"){
                        outstream << programString;
                    }
                    else if(line.trimmed() == "%DECLARE_VARIABLES%"){
                        outstream << varDeclString;
                    }
                    else if(line.trimmed() == "%DEFINE_MAX_DATA_SIZE%"){
                        outstream << maxDataDefineString;
                    }
                    else if(line.trimmed() == "%DEFINE_CHECKSUM%"){
                        outstream << checksumDefineString;
                    }
                    else if(line.trimmed() == "%DEFINE_VARIABLES%"){
                        outstream << varDefineString;
                    }
                    else{
                        outstream << line << endl;
                    }

                }
                descFile.close();
            }
            sourceFile.close();
        }
    }

    if(errors)
    {
        emit onCompileFail();
    }
    else
    {
        emit onCompileSuccess();
    }

    return true;
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
                varType = var["returnType"].toString();
                break;
            }
        }

        if(argsSize>0){
            result = QString("set"+varType+"("+root["name"].toString());
        }
        else{
            result = QString("get"+varType+"("+root["name"].toString());
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
        if(!paramSplit.at(0).compare("VARIABLE"))
        {
            QString varType;
            foreach (const QJsonValue & var, variables)
            {
                if(!var["name"].toString().compare(paramSplit.at(1))){
                    varType = var["returnType"].toString();
                    break;
                }
            }
            result.append("get"+varType+"("+paramSplit.at(1)+")");
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
