#include "Device.h"

#include <QRandomGenerator>

Device::Device(QObject* parent)
    : QObject(parent)
    , frequency(2)
{
    serial.setPortName("/dev/ttyUSB1");

    serial.setBaudRate(QSerialPort::Baud9600);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);

    connect(&serial, &QSerialPort::readyRead, this, &Device::handleReadyRead);
    connect(&serial,
            QOverload<QSerialPort::SerialPortError>::of(&QSerialPort::errorOccurred),
            this,
            &Device::handleError);

    if (serial.open(QIODevice::ReadWrite)) {
        qDebug() << "DEVICE: Serial port opened successfully";
    }
    else {
        qWarning() << "DEVICE: Failed to open serial port:" << serial.errorString();
        return;
    }

    frequencyTimer.setInterval(1000 / frequency);
    connect(&frequencyTimer, &QTimer::timeout, this, &Device::sendMeasurementData);
}

Device::~Device()
{
    serial.close();
}

void Device::handleReadyRead()
{
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

        const QString text = QString("$2,") + QString::number(frequency) + "," + QVariant(debug).toString() + ",ok\n";
        QByteArray dataToWrite = text.toStdString().c_str();
        serial.write(dataToWrite);
    }
}

void Device::handleError(QSerialPort::SerialPortError error)
{
    qDebug() << "DEVICE: Serial port error:" << error;
}

void Device::sendMeasurementData()
{
    double min = 0.0;
    double max = 1000.0;

    qlonglong randomInt1 = QRandomGenerator::global()->bounded(static_cast<qlonglong>(min * 100),
                                                               static_cast<qlonglong>(max * 100));
    double random1 = static_cast<double>(randomInt1) / 100.0;

    qlonglong randomInt2 = QRandomGenerator::global()->bounded(static_cast<qlonglong>(min * 100),
                                                               static_cast<qlonglong>(max * 100));
    double random2 = static_cast<double>(randomInt2) / 100.0;

    qlonglong randomInt3 = QRandomGenerator::global()->bounded(static_cast<qlonglong>(min * 100),
                                                               static_cast<qlonglong>(max * 100));
    double random3 = static_cast<double>(randomInt3) / 100.0;

    const auto data = QString("$") + QString::number(random1, 'f', 1) + "," + QString::number(random2, 'f', 1) + ","
                    + QString::number(random3, 'f', 1) + "\n";


    QByteArray dataToWrite = data.toLocal8Bit();
    serial.write(dataToWrite);
    qDebug() << "DEVICE: dataSent" << data;
}

void Device::startSendingMessages()
{
    frequencyTimer.start();
}

void Device::stopSendingMessages()
{
    frequencyTimer.stop();
}
