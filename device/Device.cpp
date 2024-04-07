#include "Device.h"

Device::Device(QObject *parent) : QObject(parent), frequency(2) {

  serial.setPortName("/dev/ttyUSB1");

  serial.setBaudRate(QSerialPort::Baud9600);
  serial.setDataBits(QSerialPort::Data8);
  serial.setParity(QSerialPort::NoParity);
  serial.setStopBits(QSerialPort::OneStop);

  connect(&serial, &QSerialPort::readyRead, this, &Device::handleReadyRead);
  connect(
      &serial,
      QOverload<QSerialPort::SerialPortError>::of(&QSerialPort::errorOccurred),
      this, &Device::handleError);

  if (serial.open(QIODevice::ReadWrite)) {
    qDebug() << "DEVICE: Serial port opened successfully";

  } else {
    qWarning() << "DEVICE: Failed to open serial port:" << serial.errorString();
  }

  frequencyTimer.setInterval(1000 / frequency);
  connect(&frequencyTimer, &QTimer::timeout, this,
          &Device::sendMeasurementData);
}

void Device::handleReadyRead() {
  QByteArray data = serial.readAll();
  if (data == "$0\n") {
    qDebug() << "DEVICE: start sending data";
    QByteArray dataToWrite = "$0,ok\n";
    serial.write(dataToWrite);

    startSendingMessages();
  }
  if (data == "$1\n") {
    qDebug() << "DEVICE: stop sending data";

    stopSendingMessages();

    QByteArray dataToWrite = "$1,ok\n";
    serial.write(dataToWrite);
  }

  QRegularExpression regex(R"(^\$2,(\d+),(true|false)\n$)");
  QRegularExpressionMatch match = regex.match(data);

  if (match.hasMatch()) {
    frequency = match.captured(1).toInt();
    debug = QVariant(match.captured(2)).toBool();
    if (frequencyTimer.isActive()) {
      frequencyTimer.stop();
      frequencyTimer.setInterval(1000 / frequency);
      frequencyTimer.start();
    }
    frequencyTimer.setInterval(1000 / frequency);

    qDebug() << "DEVICE: frequency" << frequency << "debug" << debug;

    const QString text = QString("$2,") + QString::number(frequency) + "," +
                         QVariant(debug).toString() + ",ok\n";
    QByteArray dataToWrite = text.toStdString().c_str();
    serial.write(dataToWrite);
  }
}

void Device::handleError(QSerialPort::SerialPortError error) {
  qWarning() << "DEVICE: Serial port error:" << error << "-"
             << serial.errorString();
}

void Device::sendMeasurementData() {
  QByteArray dataToWrite = "$123.4,567.8,999.9\n";
  serial.write(dataToWrite);
  qDebug() << "DEVICE: dataSent";
}

void Device::startSendingMessages() { frequencyTimer.start(); }
void Device::stopSendingMessages() { frequencyTimer.stop(); }
