#include "Device.h"

#include <QRandomGenerator>

Device::Device(QObject* parent)
    : QObject(parent)
    , frequency(2)
{
    serialPort.setPortName("/dev/ttyUSB1");

    serialPort.setBaudRate(QSerialPort::Baud9600);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);

    connect(&serialPort, &QSerialPort::readyRead, this, &Device::handleReadyRead);

    frequencyTimer.setInterval(1000 / frequency);
    connect(&frequencyTimer, &QTimer::timeout, this, &Device::sendMeasurementData);
}

Device::~Device()
{
    if (serialPort.isOpen()) {
        serialPort.close();
    }
}

void Device::handleReadyRead()
{
    QByteArray data = serialPort.readAll();
    if (data.startsWith("$0")) {
        if (data == "$0\n") {
            qDebug() << "DEVICE: start sending data";
            QByteArray dataToWrite = "$0,ok\n";
            serialPort.write(dataToWrite);

            startSendingMessages();
        }
        else {
            qWarning() << "DEVICE: INVALID COMMAND" << data;
            QByteArray dataToWrite = "$0,invalid command\n";
            serialPort.write(dataToWrite);
        }
    }

    if (data.startsWith("$1")) {
        if (data == "$1\n") {
            qDebug() << "DEVICE: stop sending data";

            stopSendingMessages();

            QByteArray dataToWrite = "$1,ok\n";
            serialPort.write(dataToWrite);
        }
        else {
            qWarning() << "DEVICE: INVALID COMMAND" << data;
            QByteArray dataToWrite = "$1,invalid command\n";
            serialPort.write(dataToWrite);
        }
    }
    if (data.startsWith("$2")) {
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

            qDebug() << "DEVICE: SETTING NEW frequency" << frequency << "debug" << debug;

            const QString text = QString("$2,") + QString::number(frequency) + "," + QVariant(debug).toString()
                               + ",ok\n";
            QByteArray dataToWrite = text.toStdString().c_str();
            serialPort.write(dataToWrite);
        }
        else {
            qWarning() << "DEVICE: INVALID COMMAND" << data;
            QByteArray dataToWrite = "$2,invalid command\n";
            serialPort.write(dataToWrite);
        }
    }
    if (!data.startsWith("$0") && !data.startsWith("$1") && !data.startsWith("$2")) {
        qWarning() << "DEVICE: UNKNOWN COMMAND" << data;
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
    serialPort.write(dataToWrite);
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

bool Device::openUARTConnection()
{
    if (serialPort.open(QIODevice::ReadWrite)) {
        qDebug() << "DEVICE: Serial port opened successfully";
        return true;
    }
    qWarning() << "DEVICE: Failed to open serial port:" << serialPort.errorString();
    return false;
}
