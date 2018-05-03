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

enum MonitorCommands : quint8
{
    GetNodeValue = 0x16,
    GetProgramChecksum = 0x17,
    SetForcingEnabled = 0x18,
    SetNodeForceEnable = 0x19,
    SetNodeForceDisable = 0x20,
    GetNodeForceStatus = 0x21,
    WriteDataToExternalFlash = 0x22,
    ReadDataFromExternalFlash = 0x23
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
    sendRequestForNodeData(quint8 id);

    void
    sendRequestForForceEnable();

    void
    sendRequestForForceDisable();

    void
    sendRequestForChecksum();

    void
    sendRequestForDisconnection();

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

    void
    onReplyFromDevice(quint16 id, quint8 command, quint8 param, quint32 data);

    //----
    void
    onChecksumRecieved(quint32 checksum);

    void
    onNodeDataRecieved(quint8 id, quint32 data);

    void
    onForceEnabled();

    void
    onNodeDataForcedStatus(quint8 id, bool forced, quint32 data);

    void
    onDataWrittenToExternalFlash(quint32 addr, quint8 data);

    void
    onDataReadFromExternalFlash(quint32 addr, quint8 data);

private:

    void
    receiveData();

    void
    write(const QByteArray &writeData);

public slots:

    void
    forceNodeDataValue(quint8 id, bool force, quint32 data);

    void
    getNodeDataForcedStatus(quint8 id);

    void
    sendRequestToDevice(DeviceDataFrame request);

    void
    sendRequestToDevice(quint8 command, quint8 param, quint32 data);

    void
    writeDataToExternalFlash(quint32 addr, quint8 data);

    void
    readDataFromExternalFlash(quint32 addr);

private slots:

    void
    handleBytesWritten(qint64 bytes);

    void
    handleWriteTimeout();

    void
    handleWriteTimerTimeout();

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
    QList<QByteArray> dataToSend;
    QByteArray writeData;
    qint64 bytesWritten = 0;
    quint8 requestCounter = 0;
};

#endif // MONITORPLUGIN_H
