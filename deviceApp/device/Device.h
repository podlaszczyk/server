#pragma once

#include <QSerialPort>
#include <QtCore>

class Device : public QObject
{
    Q_OBJECT

public:
    explicit Device(QObject* parent = nullptr);
    ~Device() override;

    bool openUARTConnection();

private slots:
    void handleReadyRead();

    void handleError(QSerialPort::SerialPortError error);
    void sendMeasurementData();
    void startSendingMessages();
    void stopSendingMessages();

private:
    QSerialPort serialPort;
    QTimer frequencyTimer;
    QTimer requestTimer;
    int frequency;
    bool debug = false;
};
