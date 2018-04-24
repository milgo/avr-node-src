#include "MonitorPlugin.hpp"

#include <QFile>
#include <QJsonArray>
#include <QProcess>
#include "hdlc_qt.hpp"

MonitorPlugin::
MonitorPlugin()
{
    HDLC_qt* hdlc = HDLC_qt::instance();

    QObject::connect(hdlc, qOverload<char>(&HDLC_qt::hdlcTransmitByte), this, qOverload<char>(&MonitorPlugin::transmitByte));
    QObject::connect(hdlc, qOverload<QByteArray>(&HDLC_qt::hdlcTransmitByte), this, qOverload<QByteArray>(&MonitorPlugin::transmitByte));
    QObject::connect(hdlc, &HDLC_qt::hdlcValidFrameReceived, this, &MonitorPlugin::replyHandler);
    QObject::connect(this, &MonitorPlugin::dataReceived, hdlc, &HDLC_qt::charReceiver);
    QObject::connect(this, &MonitorPlugin::sendDataFrame, hdlc, &HDLC_qt::frameDecode);

    writeTimeoutTimer.setSingleShot(true);
    startWriteTimer.setSingleShot(true);

    QObject::connect(&serialPort, &QSerialPort::readyRead, this, &MonitorPlugin::receiveData);
    QObject::connect(&serialPort, &QSerialPort::bytesWritten, this, &MonitorPlugin::handleBytesWritten);
    QObject::connect(&serialPort, &QSerialPort::errorOccurred, this, &MonitorPlugin::handleError);

    QObject::connect(&writeTimeoutTimer, &QTimer::timeout, this, &MonitorPlugin::handleWriteTimeout);
    QObject::connect(&writeTimer, &QTimer::timeout, this, &MonitorPlugin::handleWriteTimerTimeout);

    QObject::connect(&startWriteTimer, &QTimer::timeout, [&](){
        writeTimer.start(10);
        LOG("starting writer timer");
        emit onConnectedToDevice();
        sendRequestForChecksum();
    });
}

QObject*
MonitorPlugin::
getObject ()
{
    return this;
}

void
MonitorPlugin::
transmitByte(char data)
{
    char t[1];
    t[0]=data;
    dataToSend.append(QByteArray(t, 1));
    LOGV("sending data to queue [%1]", data);
}

void
MonitorPlugin::
transmitByte(QByteArray data)
{
    dataToSend.append(data);
    LOGV("sending data to queue [%1]", QString(data.toHex()));
}

void
MonitorPlugin::
replyHandler(QByteArray buffer, quint16 bytes_received)
{
    QDataStream stream(buffer);
    stream.setVersion(QDataStream::Qt_4_5);
    stream.setByteOrder(QDataStream::LittleEndian);
    DeviceDataFrame ddf;

    stream >> ddf.id
            >> ddf.command
            >> ddf.param
            >> ddf.data;

    LOGV("received replay with id=%1", ddf.id);

    switch(ddf.command){
    case MonitorCommands::GetNodeForceStatus:
    {
        emit onNodeDataForcedStatus(ddf.id, ddf.param==1, ddf.data);
        break;
    }
    case MonitorCommands::GetProgramChecksum:
    {
        emit onChecksumRecieved(ddf.data);
        break;
    }
    case MonitorCommands::SetForcingEnabled:
    {
        if(ddf.param == 1)
        {
          qDebug("Setting force enabled on device");
          //emit mainWidget.acquireNodeValues();
          emit onForceEnabled();
        }
        else
        {
          qDebug("Setting force disable on device");
          disconnectFromDevice();
        }
        break;
    }
    case MonitorCommands::GetNodeValue:
    {
        qDebug(QString("[main.cpp] recieved value for node(%1) = [%2]").arg(QString::number(ddf.param, 16), QString::number(ddf.data, 16)).toUtf8());
        quint16 id = ddf.param;
        quint32 value = ddf.data;
        emit onNodeDataRecieved(id, value);
        break;
    }
    }

    emit onReplyFromDevice(ddf);
}

bool
MonitorPlugin
::connectToDevice(QJsonObject connectionInfo)
{
    serialPort.setPortName(connectionInfo["comPort"].toString());
    serialPort.setBaudRate(QSerialPort::Baud19200);

    LOGV("connecting to device using port %1", connectionInfo["comPort"].toString());
    if (!serialPort.open(QIODevice::ReadWrite))
    {
        LOG("connection error");
        return false;
    }

    LOG("device connected");
    emit onInfoMessage("Device connected...");

    startWriteTimer.start(5000);
    return true;
}

void
MonitorPlugin
::sendRequestToDevice(DeviceDataFrame request)
{
    request.id = requestCounter;
    //requestList[requestCounter] = request;
    requestCounter++;
    QByteArray byteArray;

    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setVersion(QDataStream::Qt_4_5);

    stream << request.id
           << request.command
           << request.param
           << request.data;

    //LOGV("repeating request with id=%1", request.id);
    LOGV("adding request with id=%1 to queue", request.id);
    emit sendDataFrame(byteArray, byteArray.size());
}

void
MonitorPlugin
::sendRequestToDevice(quint8 command, quint8 param, quint32 data)
{
    DeviceDataFrame ddf;
    ddf.command = command;
    ddf.param = param;
    ddf.data = data;
    sendRequestToDevice(ddf);
}

bool
MonitorPlugin
::disconnectFromDevice()
{
    //send unforce first !!!!!!!!!!

    writeTimer.stop();
    dataToSend.clear();
    writeData.clear();
    bytesWritten = 0;

    if (serialPort.isOpen())
    {
        serialPort.close();
        LOG("device disconnected");
        emit onInfoMessage("Device disconnected...");
        emit onDisconnectedFromDevice();
        return true;
    }    
    return false;
}

bool
MonitorPlugin
::isConnected()
{
    return serialPort.isOpen();
}

void
MonitorPlugin
::receiveData()
{
    QByteArray data = serialPort.readAll();
    LOGV("received data: [%1]", QString(data.toHex()));
    emit dataReceived(data);
}

void
MonitorPlugin
::write(const QByteArray &writeData)
{
    LOGV("writing data to port [%1]", QString(writeData.toHex()));
    this->writeData.append(writeData);

    const qint64 bytesWritten = serialPort.write(writeData);

    if (bytesWritten == -1) {
        emit onErrorMessage(QObject::tr("Failed to write all the data to port %1, error: %2")
                             .arg(serialPort.portName())
                             .arg(serialPort.errorString()));
    } else if (bytesWritten != writeData.size()) {
        emit onErrorMessage(QObject::tr("Failed to write all the data to port %1, error: %2")
                             .arg(serialPort.portName())
                             .arg(serialPort.errorString()));
    }

    this->writeTimeoutTimer.start(5000);
}

void
MonitorPlugin
::handleBytesWritten(qint64 bytes)
{
    this->bytesWritten += bytes;
    if (this->bytesWritten == this->writeData.size())
    {
        this->bytesWritten = 0;
        this->writeData.clear();
        LOG("all bytes sended");
        writeTimeoutTimer.stop();
    }
}

void
MonitorPlugin
::handleWriteTimeout()
{
    LOG("write to device timeout");
    emit onErrorMessage(QObject::tr("Operation timed out for port %1, error: %2")
                         .arg(serialPort.portName())
                         .arg(serialPort.errorString()));
}

void
MonitorPlugin
::handleWriteTimerTimeout()
{
    if(dataToSend.size() > 0)
    {
        LOGV("time to send some data from queue, writing [%1]", QString(dataToSend.first().toHex()));
        QByteArray data = dataToSend.first();
        dataToSend.removeFirst();
        write(data);

    }
    else{
        //LOG("time to send some data from queue, but there isn't any");
    }
}

void
MonitorPlugin
::handleError(QSerialPort::SerialPortError serialPortError)
{
    LOGV("serial port error occured: %1", serialPortError);
    if (serialPortError == QSerialPort::WriteError)
    {
        emit onErrorMessage(QObject::tr("An I/O error occurred while writing the data to port %1, error: %2")
                            .arg(serialPort.portName())
                            .arg(serialPort.errorString()));
    }
    else if (serialPortError == QSerialPort::ReadError)
    {
        emit onErrorMessage(QObject::tr("An I/O error occurred while reading the data from port %1, error: %2")
                            .arg(serialPort.portName())
                            .arg(serialPort.errorString()));
    }
}

void
MonitorPlugin::
sendRequestForDisconnection(){
    sendRequestForForceDisable();
}

void
MonitorPlugin::
sendRequestForChecksum(){
    sendRequestToDevice(MonitorCommands::GetProgramChecksum, 0, 0);
}

void
MonitorPlugin::
sendRequestForForceEnable(){
    sendRequestToDevice(MonitorCommands::SetForcingEnabled, 1, 0);
}

void
MonitorPlugin::
sendRequestForForceDisable(){
    sendRequestToDevice(MonitorCommands::SetForcingEnabled, 0, 0);
}

void
MonitorPlugin::
forceNodeDataValue(quint8 id, bool force, quint32 data){
    if(force)
        sendRequestToDevice(MonitorCommands::SetNodeForceEnable, id, data);
    else
        sendRequestToDevice(MonitorCommands::SetNodeForceDisable, id, 0);
}

void
MonitorPlugin::
getNodeDataForcedStatus(quint8 id){
    sendRequestToDevice(MonitorCommands::GetNodeForceStatus, id, 0);
}

void
MonitorPlugin::
sendRequestForNodeData(quint8 id){
    sendRequestToDevice(MonitorCommands::GetNodeValue, id, 0);
}
