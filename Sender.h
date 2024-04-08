#pragma once

#include <QSerialPort>
#include <QTimer>
#include <QtCore>

class Sender : public QObject {
  Q_OBJECT

public:

  struct Data{
    double pressure;
    double temperature;
    double velocity;
  };

  struct Config{
    int frequency=10;
    bool debug=false;
  };
  Sender(QObject *parent = nullptr);

  void sendRequest(const QByteArray &request);

  Config getConfig();
  std::vector<Data> getMeasurements(int number);
  Data getLatest();
  Data getMeanLast10();

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

  std::vector<Data> measurements;
  Config config;
};
