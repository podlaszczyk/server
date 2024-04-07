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

//  connect(&serial, &QSerialPort::readyRead, this, &Sender::readMeasurement);
  connect(
      &serial,
      QOverload<QSerialPort::SerialPortError>::of(&QSerialPort::errorOccurred),
      this, &Sender::handleError);
}

QSerialPort::SerialPortError Sender::handleReadyRead() {
  QByteArray data = serial.readAll();
  qDebug() << "data" << data;
  if (data == "$0,ok\n") {
    qDebug() << "SENDER: CORRECT START RESPONSE ";
    return QSerialPort::SerialPortError::NoError;
  }
  if (data == "$1,ok\n") {
    qDebug() << "SENDER: CORRECT STOP RESPONSE ";
    return QSerialPort::SerialPortError::NoError;
  }
  return QSerialPort::SerialPortError::UnknownError;
}

QSerialPort::SerialPortError Sender::startSending() {
  QByteArray dataToWrite = "$0\n";
  qint64 bytesWritten = serial.write(dataToWrite);
  if (bytesWritten == -1) {
    qCritical() << "Failed to write to serial port:" << serial.errorString();
    serial.close();
  }

  if (serial.waitForReadyRead(timeout)) {
    auto result = handleReadyRead();
    return result;
  }
  return QSerialPort::SerialPortError::TimeoutError;
}

QSerialPort::SerialPortError Sender::stopSending() {

  QByteArray dataToWrite = "$1\n";
  qint64 bytesWritten = serial.write(dataToWrite);
  serial.clear(QSerialPort::Direction::Input);
  if (bytesWritten == -1) {
    qCritical() << "Failed to write to serial port:" << serial.errorString();
    serial.close();
  }

  if (serial.waitForReadyRead(timeout)) {
    auto result = handleReadyRead();
    return result;
  }
  return QSerialPort::SerialPortError::TimeoutError;
}
//void Sender::readMeasurement() {
//  QByteArray data = serial.readLine();
//  qDebug() << "data" << data;
//}

void Sender::handleError(QSerialPort::SerialPortError error) {
  qWarning() << " Serial port error:" << error << "-" << serial.errorString();
}