#pragma once

#include <QSerialPort>
#include <QtCore>

class Device : public QObject
{
    Q_OBJECT

public:
    explicit Device(QObject* parent = nullptr);

private slots:
    void handleReadyRead();

    void handleError(QSerialPort::SerialPortError error);
    void sendMeasurementData();
    void startSendingMessages();
    void stopSendingMessages();

private:
    QSerialPort serial;
    QTimer frequencyTimer;
    QTimer requestTimer;
    int frequency;
    bool debug = false;
};
