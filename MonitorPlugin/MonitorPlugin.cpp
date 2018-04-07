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
    QObject::connect(hdlc, &HDLC_qt::hdlcValidFrameReceived, this, &MonitorPlugin::commandRouter);
    QObject::connect(this, &MonitorPlugin::dataReceived, hdlc, &HDLC_qt::charReceiver);
    QObject::connect(this, &MonitorPlugin::sendDataFrame, hdlc, &HDLC_qt::frameDecode);

    writeTimeoutTimer.setSingleShot(true);
    QObject::connect(&serialPort, &QSerialPort::readyRead, this, &MonitorPlugin::receiveData);
    QObject::connect(&serialPort, &QSerialPort::bytesWritten, this, &MonitorPlugin::handleBytesWritten);
    QObject::connect(&serialPort, &QSerialPort::errorOccurred, this, &MonitorPlugin::handleError);
    QObject::connect(&writeTimeoutTimer, &QTimer::timeout, this, &MonitorPlugin::handleWriteTimeout);
    QObject::connect(&writeTimer, &QTimer::timeout, this, &MonitorPlugin::handleWriteTimer);
    writeTimer.start(500);
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
}

void
MonitorPlugin::
transmitByte(QByteArray data)
{
    dataToSend.append(data);
}

void
MonitorPlugin::
commandRouter(QByteArray buffer, quint16 bytes_received)
{
    /*enum serial_responses command = static_cast<serial_responses>(buffer.at(0));
    switch(command)
    {
        case RESPONSE_ERROR:            this->command_error(); break;
        case RESPONSE_VERSION:          this->command_default(buffer, bytes_received); break;
        case RESPONSE_BUTTON_PRESS:  	this->command_default(buffer, bytes_received); break;
        case RESPONSE_READ_SIGNATURE:   this->response_read_signature(buffer, bytes_received); break;
        case RESPONSE_ECHO_DATA:        this->command_default(buffer, bytes_received); break;
        default:
            this->command_error();
            break;
    }*/

    qDebug(buffer.left(bytes_received));
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
::receiveData()
{
    QByteArray data = serialPort.readAll();
    emit dataReceived(data);
}

void
MonitorPlugin
::acquireProgramChecksum()
{
    /*QByteArray data;

    data.append((char)0);
    data.append((char)0);
    data.append((char)0);
    data.append((char)0);

    dataToSend.append(data);*/


}

void
MonitorPlugin
::acquireValue(QString id)
{
    /*QByteArray data;
    unsigned int n = id.toInt();

    data.append((char)(n & 0x00FF));
    data.append((char)((n & 0xFF00) >> 8));
    data.append((char)1);
    data.append((char)0);

    qDebug("sending="+data.toHex());

    dataToSend.append(data);*/

    emit sendDataFrame(QByteArray("hello", 5), 5);
}

void
MonitorPlugin
::write(const QByteArray &writeData)
{
    qDebug("writing: " + writeData.toHex());
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
