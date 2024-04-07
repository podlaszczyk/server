#pragma once

#include <QSerialPort>
#include <QTimer>
#include <QtCore>

class Sender : public QObject {
  Q_OBJECT

public:
  Sender(QObject *parent = nullptr);

  QSerialPort::SerialPortError handleReadyRead();
  QSerialPort::SerialPortError startSending();
  QSerialPort::SerialPortError stopSending();

signals:
  void dataHandled();

private:
  QSerialPort serial;
  int timeout = 3000;

  void readMeasurement();
  void handleError(QSerialPort::SerialPortError error);


};
