#pragma once

#include <QSerialPort>
#include <QTimer>
#include <QtCore>

class Sender : public QObject {
  Q_OBJECT

public:
  Sender(QObject *parent = nullptr);

//  QSerialPort::SerialPortError handleReadyRead();
//  QSerialPort::SerialPortError startSending();
//  QSerialPort::SerialPortError stopSending();

  void sendRequest(const QByteArray &request);
signals:
  void dataHandled();
  void requestResult(const QString &result);
private:
  QSerialPort serial;
  int timeout = 3000;
  QTimer timer;

  void readMeasurement();
  void handleError(QSerialPort::SerialPortError error);
  void handleReadyRead();
  void handleTimeout();
  void processMessage(const QByteArray &message);
};
