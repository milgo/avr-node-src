#include "MonitorPlugin.hpp"

#include <QFile>
#include <QJsonArray>
#include <QProcess>

MonitorPlugin::
MonitorPlugin()
{
    writeTimeoutTimer.setSingleShot(true);
    QObject::connect(&serialPort, &QSerialPort::readyRead, this, &MonitorPlugin::reciveData);
    QObject::connect(&serialPort, &QSerialPort::bytesWritten, this, &MonitorPlugin::handleBytesWritten);
    QObject::connect(&serialPort, &QSerialPort::errorOccurred, this, &MonitorPlugin::handleError);
    QObject::connect(&writeTimeoutTimer, &QTimer::timeout, this, &MonitorPlugin::handleWriteTimeout);
    QObject::connect(&writeTimer, &QTimer::timeout, this, &MonitorPlugin::handleWriteTimer);
    //writeTimer.start(500);
}

QObject*
MonitorPlugin::
getObject ()
{
    return this;
}

bool
MonitorPlugin
::connectToDevice(QJsonObject connectionInfo)
{
    serialPort.setPortName(connectionInfo["comPort"].toString());
    serialPort.setBaudRate(QSerialPort::Baud19200);

    if (!serialPort.open(QIODevice::ReadWrite))
    {
        return false;
    }
    emit onInfoMessage("Device connected...");
    writeTimer.start(500);
    return true;
}

bool
MonitorPlugin
::disconnectFromDevice()
{
    if (serialPort.isOpen())
    {
        serialPort.close();
        emit onInfoMessage("Device disconnected...");
        writeTimer.stop();
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
::reciveData()
{
    readData.append(serialPort.readAll());
    qDebug("recive="+readData.toHex());
    while(readData.size()>=8)
    {
        QByteArray response = readData.left(4);
        readData.remove(0,4);
        QByteArray data = readData.left(4);
        readData.remove(0,4);

        QDataStream ds(response);
        uint res = -1;
        ds>>res;

        uint returnCode = res & 0x0000FFFF;
        uint returnId = (res & 0xFFFF0000) >> 16;

        qDebug(QString("response code(" +QString::number(returnCode)+"), id("+QString::number(returnId)+")").toUtf8());

        switch(returnCode)
        {
        case 0:{
            emit programChecksumAcquired(data);
            break;
        }
        case 1:{
            emit valueAcquired(QString::number(returnId), data);
            break;
        }
        }
    }
    //readData.clear();
}

void
MonitorPlugin
::acquireProgramChecksum()
{
    QByteArray data;

    data.append((char)0);
    data.append((char)0);
    data.append((char)0);
    data.append((char)0);

    dataToSend.append(data);
}

void
MonitorPlugin
::acquireValue(QString id)
{
    QByteArray data;
    unsigned int n = id.toInt();

    data.append((char)(n & 0x00FF));
    data.append((char)((n & 0xFF00) >> 8));
    data.append((char)1);
    data.append((char)0);

    qDebug("sending="+data.toHex());

    dataToSend.append(data);
}

void
MonitorPlugin
::write(const QByteArray &writeData)
{
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
        writeTimeoutTimer.stop();
    }
}

void
MonitorPlugin
::handleWriteTimeout()
{
    emit onErrorMessage(QObject::tr("Operation timed out for port %1, error: %2")
                         .arg(serialPort.portName())
                         .arg(serialPort.errorString()));
}

void
MonitorPlugin
::handleWriteTimer()
{
    if(dataToSend.size() > 0)
    {
        write(dataToSend.first());
        dataToSend.removeFirst();
    }
}

void
MonitorPlugin
::handleError(QSerialPort::SerialPortError serialPortError)
{
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
