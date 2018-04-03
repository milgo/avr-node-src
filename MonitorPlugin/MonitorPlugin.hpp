#ifndef MONITORPLUGIN_H
#define MONITORPLUGIN_H

#include <QtPlugin>
#include <QObject>

#include <queue>
#include <QtCore/QObject>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QDataStream>

#include "MonitorPlugin_global.hpp"
#include "IMonitorPlugin.hpp"

class MONITORPLUGINSHARED_EXPORT MonitorPlugin : public QObject, IMonitorPlugin
{
    Q_OBJECT
    Q_INTERFACES(IMonitorPlugin)
    Q_PLUGIN_METADATA(IID "com.monitorplugin")

public:

    explicit MonitorPlugin();

    QObject* getObject() override;

    void
    acquireValue(QString id) override;

    bool
    connectToDevice(QJsonObject connectionInfo) override;

    bool
    disconnectFromDevice() override;

    bool
    isConnected() override;

    void
    acquireProgramChecksum();

signals:

    void
    valueAcquired(QString id, QByteArray value) override;

    void
    programChecksumAcquired(QByteArray value);

    void
    onInfoMessage(QString message) override;

    void
    onErrorMessage(QString message) override;

private:

    void
    reciveData();

    void
    write(const QByteArray &writeData);

private slots:

    void
    handleBytesWritten(qint64 bytes);

    void
    handleWriteTimeout();

    void
    handleError(QSerialPort::SerialPortError error);

private:

    QSerialPort serialPort;
    QTimer writeTimer;

    std::queue<QString> variablesStack;

    QByteArray writeData;
    QByteArray readData;

    qint64 bytesWritten = 0;
};

#endif // MONITORPLUGIN_H
