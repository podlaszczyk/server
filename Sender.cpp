#include "Sender.h"
#include <QThread>

Sender::Sender(QObject *parent) : QObject(parent) {

  serial.setPortName("/dev/ttyUSB0");

  serial.setBaudRate(QSerialPort::Baud9600);
  serial.setDataBits(QSerialPort::Data8);
  serial.setParity(QSerialPort::NoParity);
  serial.setStopBits(QSerialPort::OneStop);

  if (serial.open(QIODevice::ReadWrite)) {
    qDebug() << "Serial port MAIN opened successfully";

  } else {
    qWarning() << "Failed to MAIN open serial port:" << serial.errorString();
  }

  connect(
      &serial,
      QOverload<QSerialPort::SerialPortError>::of(&QSerialPort::errorOccurred),
      this, &Sender::handleError);

  connect(&serial, &QSerialPort::readyRead, this, &Sender::handleReadyRead);
  connect(&timer, &QTimer::timeout, this, &Sender::handleTimeout);
}

void Sender::handleReadyRead() {
  QByteArray data = serial.readAll();
  qDebug() << "Received data:" << data;

  QList<QByteArray> messages = data.split('\n');
  for (const QByteArray &message : messages) {
    if (message.isEmpty())
      continue;

    processMessage(message);
  }
}

void Sender::processMessage(const QByteArray &message) {
  QRegularExpression regexMeasurement(R"(^\$([\d.]+),([\d.]+),([\d.]+)$)");
  QRegularExpressionMatch matchMeasurement = regexMeasurement.match(message);

  QRegularExpression regexConfiguration(R"(^\$2,(\d+),(true|false),(\w+)$)");
  QRegularExpressionMatch matchConfiguration =
      regexConfiguration.match(message);

  if (message.startsWith('$')) {
    if (message.startsWith("$0,ok")) {

      qDebug() << "Received start response:" << message;
      timer.stop();
      emit requestResult("ok");

    } else if (message.startsWith("$1,ok")) {

      qDebug() << "Received stop response:" << message;
      timer.stop();
      emit requestResult("ok");

    } else if (matchConfiguration.hasMatch()) {

      const auto frequency = matchConfiguration.captured(1).toInt();
      const bool debug = matchConfiguration.captured(2) == "true";
      const auto status = matchConfiguration.captured(3);
      qDebug() << "Received config response:"
               << "frequency" << frequency << "debug" << debug << "status"
               << status;
      timer.stop();
      emit requestResult(status);

    } else if (matchMeasurement.hasMatch()) {

      const auto pressure = matchMeasurement.captured(1).toDouble();
      const auto temperature = matchMeasurement.captured(2).toDouble();
      const auto velocity = matchMeasurement.captured(3).toDouble();
      qDebug() << "P" << pressure << "T" << temperature << "V" << velocity;

    }
  } else {
    qDebug() << "Invalid message format:" << message;
  }
}

void Sender::handleTimeout() {
  qDebug() << "Request timed out";
  timer.stop();
  emit requestResult("Error: Request timed out");
}

void Sender::sendRequest(const QByteArray &request) {
  serial.write(request);
  timer.start(1000); // Timeout after 1 second (adjust as needed)
}

void Sender::handleError(QSerialPort::SerialPortError error) {
  qWarning() << " Serial port error:" << error << "-" << serial.errorString();
}