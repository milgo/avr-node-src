#include "MonitorPlugin.hpp"

#include <QFile>
#include <QJsonArray>
#include <QProcess>

MonitorPlugin::
MonitorPlugin()
{
    writeTimer.setSingleShot(true);
    QObject::connect(&serialPort, &QSerialPort::readyRead, this, &MonitorPlugin::reciveData);
    QObject::connect(&serialPort, &QSerialPort::bytesWritten, this, &MonitorPlugin::handleBytesWritten);
    QObject::connect(&serialPort, &QSerialPort::errorOccurred, this, &MonitorPlugin::handleError);
    QObject::connect(&writeTimer, &QTimer::timeout, this, &MonitorPlugin::handleWriteTimeout);
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
        emit onInfoMessage("Device connected...");
        return false;
    }
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
    while(readData.size()>=8)
    {
        QByteArray response = readData.left(4);
        readData.remove(0,4);
        QByteArray data = readData.left(4);
        readData.remove(0,4);

        QDataStream ds(response);
        uint res = -1;
        ds>>res;

        switch(res)
        {
        case 0:{
            emit programChecksumAcquired(data);
            break;
        }
        case 1:{
            QString id = variablesStack.front();
            variablesStack.pop();
            emit valueAcquired(id, data);
            break;
        }
        }
    }
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

    write(data);
}

void
MonitorPlugin
::acquireValue(QString id)
{
    QByteArray data;
    unsigned int n = id.toInt();

    data.append((char)0);
    data.append((char)1);
    data.append((char)((n & 0xFF00) >> 8));
    data.append((char)(n & 0x00FF));

    write(data);
    variablesStack.push(id);
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

    this->writeTimer.start(5000);
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
        writeTimer.stop();
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
