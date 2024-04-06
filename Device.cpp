
#include "Device.h"
void Device::handleReadyRead() {
  QByteArray data = serial.readAll();
  if (data == "$0\n") {
    qDebug() << "start sending data";
    QByteArray dataToWrite = "$0,ok\n";
    serial.write(dataToWrite);

    startSendingMessages();
  }
  if (data == "$1\n") {
    qDebug() << "stop sending data";
    QByteArray dataToWrite = "$1,ok\n";
    serial.write(dataToWrite);

    stopSendingMessages();
  }

  QRegularExpression regex("\\$(\\d+),(\\d+),(\\d+)\n");
  QRegularExpressionMatch match = regex.match(data);

  if (match.hasMatch()) {
    frequency = match.captured(2).toInt();
    debug = match.captured(3).toInt();
    if (frequencyTimer.isActive()) {
      frequencyTimer.stop();
      frequencyTimer.setInterval(1000 / frequency);
      frequencyTimer.start();
    }
    frequencyTimer.setInterval(1000 / frequency);

    qDebug() << "v1:" << frequency;
    qDebug() << "v2:" << debug;

    qDebug() << "changed configuration data";
    const QString text = QString("$2,") + QString::number(frequency) + "," +
                         QString::number(debug) + ",ok\n";
    QByteArray dataToWrite = text.toStdString().c_str();
    serial.write(dataToWrite);
  }

  emit newData(data);
}

void Device::handleError(QSerialPort::SerialPortError error) {
  qWarning() << "Serial port error:" << error << "-" << serial.errorString();
}

void Device::requestData() {
  if (serial.isOpen()) {

    qDebug() << "Requesting data...";
  }
}

void Device::sendMeasurementData() {
  QByteArray dataToWrite = "$123.4,567.8,999.9\n";
  serial.write(dataToWrite);
  qDebug() << "dataSent";
}

void Device::startSendingMessages() { frequencyTimer.start(); }
void Device::stopSendingMessages() { frequencyTimer.stop(); }
