#pragma once

#include <QSerialPort>
#include <QtCore>

class Device : public QObject {
  Q_OBJECT

public:
  explicit Device(QObject *parent = nullptr) : QObject(parent), frequency(10) {

    serial.setPortName("/dev/ttyUSB0");

    serial.setBaudRate(QSerialPort::Baud9600);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);

    connect(&serial, &QSerialPort::readyRead, this, &Device::handleReadyRead);
    connect(&serial,
            QOverload<QSerialPort::SerialPortError>::of(
                &QSerialPort::errorOccurred),
            this, &Device::handleError);

    if (serial.open(QIODevice::ReadWrite)) {
      qDebug() << "Serial port opened successfully";

    } else {
      qWarning() << "Failed to open serial port:" << serial.errorString();
    }

    // optional request timer
    connect(&requestTimer, &QTimer::timeout, this, &Device::requestData);
    requestTimer.start(1000); // Request data every 1000 ms (1 second)

    frequencyTimer.setInterval(1000 / frequency);
    connect(&frequencyTimer, &QTimer::timeout, this,
            &Device::sendMeasurementData);
  }

signals:
  void newData(const QByteArray &data);

private slots:
  void handleReadyRead();

  void handleError(QSerialPort::SerialPortError error);
  void requestData();

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
