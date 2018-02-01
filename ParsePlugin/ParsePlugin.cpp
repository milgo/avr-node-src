#include "ParsePlugin.hpp"


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
load()
{
    qDebug("load");
    emit this->onParseNewVariable(QString("VarType1"));
    emit this->onParseNewFunctionBlock(QString("Func1"), QString("VarType2"), QString("args"));
    return true;
}

void
ParsePlugin::
compile(QString program)
{
}
