#ifndef MONITORPLUGIN_H
#define MONITORPLUGIN_H

#include <QtPlugin>
#include <QObject>
#include <QMap>

#include <queue>
#include <QtCore/QObject>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QDataStream>

#include "MonitorPlugin_global.hpp"
#include "IMonitorPlugin.hpp"

#define LOG(str) qDebug("[MonitorPlugin]: " + QString(str).toUtf8())
#define LOGV(str,v) qDebug("[MonitorPlugin]: " + QString(str).arg(v).toUtf8())

struct DeviceDataFrame
{
    quint16 id;
    quint8 command;
    quint8 param;
    quint32 data;
};

class MONITORPLUGINSHARED_EXPORT MonitorPlugin : public QObject, IMonitorPlugin
{
    Q_OBJECT
    Q_INTERFACES(IMonitorPlugin)
    Q_PLUGIN_METADATA(IID "com.monitorplugin")

public:

    explicit MonitorPlugin();

    QObject* getObject() override;

    bool
    connectToDevice(QJsonObject connectionInfo) override;

    bool
    disconnectFromDevice() override;

    bool
    isConnected() override;

    void
    sendRequestToDevice(DeviceDataFrame request);

    void
    sendRequestToDevice(quint8 command, quint8 param, quint32 data);

signals:

    void
    onInfoMessage(QString message) override;

    void
    onErrorMessage(QString message) override;

    void
    onConnectedToDevice();

    void
    onDisconnectedFromDevice();

    void
    dataReceived(QByteArray data);

    void
    sendDataFrame(QByteArray buffer, quint16 bytes_to_send);

    void
    onReplyFromDevice(DeviceDataFrame reply);

private:

    void
    receiveData();

    void
    write(const QByteArray &writeData);

private slots:

    void
    handleBytesWritten(qint64 bytes);

    void
    handleWriteTimeout();

    void
    handleWriteTimerTimeout();

    void
    handleRepeatRequestTimeout();

    void
    handleError(QSerialPort::SerialPortError error);

    void
    transmitByte(char data);

    void
    transmitByte(QByteArray data);

    void
    replyHandler(QByteArray buffer, quint16 bytes_received);

private:

    QSerialPort serialPort;
    QTimer writeTimeoutTimer;
    QTimer writeTimer;
    QTimer startWriteTimer;
    //QTimer repeatRequestTimer;

    QList<QByteArray> dataToSend;
    //QMap<qint8, DeviceDataFrame> requestList;
    QByteArray writeData;
    qint64 bytesWritten = 0;
    //quint8 requestCounter = 0;
};

#endif // MONITORPLUGIN_H
