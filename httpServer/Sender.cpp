#include "Sender.h"

#include "Database.h"

#include <QThread>

Sender::Sender(const Database& db, QObject* parent)
    : QObject(parent)
    , database(db)
{
    serial.setPortName("/dev/ttyUSB0");

    serial.setBaudRate(QSerialPort::Baud9600);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);

    if (serial.open(QIODevice::ReadWrite)) {
        qDebug() << "Serial port MAIN opened successfully";
    }
    else {
        qWarning() << "Failed to MAIN open serial port:" << serial.errorString();
    }

    connect(&serial,
            QOverload<QSerialPort::SerialPortError>::of(&QSerialPort::errorOccurred),
            this,
            &Sender::handleError);

    connect(&serial, &QSerialPort::readyRead, this, &Sender::handleReadyRead);
    connect(&timer, &QTimer::timeout, this, &Sender::handleTimeout);
}

void Sender::handleReadyRead()
{
    QByteArray data = serial.readAll();
    qDebug() << "Received data:" << data;

    QList<QByteArray> messages = data.split('\n');
    for (const QByteArray& message : messages) {
        if (message.isEmpty())
            continue;

        processMessage(message);
    }
}

void Sender::processMessage(const QByteArray& message)
{
    QRegularExpression regexMeasurement(R"(^\$([\d.]+),([\d.]+),([\d.]+)$)");
    QRegularExpressionMatch matchMeasurement = regexMeasurement.match(message);

    QRegularExpression regexConfiguration(R"(^\$2,(\d+),(true|false),(\w+)$)");
    QRegularExpressionMatch matchConfiguration = regexConfiguration.match(message);

    if (message.startsWith('$')) {
        if (message.startsWith("$0,ok")) {
            qDebug() << "Received start response:" << message;
            timer.stop();
            emit requestResult("ok");
        }
        else if (message.startsWith("$1,ok")) {
            qDebug() << "Received stop response:" << message;
            timer.stop();
            emit requestResult("ok");
        }
        else if (matchConfiguration.hasMatch()) {
            const auto frequency = matchConfiguration.captured(1).toInt();
            const bool debug = matchConfiguration.captured(2) == "true";
            const auto status = matchConfiguration.captured(3);

            database.insertRecordToConfiguration({.frequency = frequency, .debug = debug});

            qDebug() << "Received config response:"
                     << "frequency" << frequency << "debug" << debug << "status" << status;
            timer.stop();
            emit requestResult(status);
        }
        else if (matchMeasurement.hasMatch()) {
            const auto pressure = matchMeasurement.captured(1).toDouble();
            const auto temperature = matchMeasurement.captured(2).toDouble();
            const auto velocity = matchMeasurement.captured(3).toDouble();
            qDebug() << "P" << pressure << "T" << temperature << "V" << velocity;

            addData({.pressure = pressure, .temperature = temperature, .velocity = velocity});
        }
    }
    else {
        qDebug() << "Invalid message format:" << message;
    }
}

void Sender::handleTimeout()
{
    qDebug() << "Request timed out";
    timer.stop();
    emit requestResult("Error: Request timed out");
}

void Sender::sendRequest(const QByteArray& request)
{
    serial.write(request);
    timer.start(1000);
}

void Sender::handleError(QSerialPort::SerialPortError error)
{
    qWarning() << " Serial port error:" << error << "-" << serial.errorString();
}

std::deque<Data> Sender::getMeasurements(int number)
{
    const auto records = database.getLatestNRecords(number);
    return records;
}

Config Sender::getConfig()
{
    const auto config = database.getConfiguration();
    if (config.has_value()) {
        return config.value();
    }
    return {};
}

Data Sender::getLatest()
{
    const auto records = database.getLatestNRecords(1);
    if (!records.empty()) {
        return records.back();
    }
    return {};
}

Data Sender::getMeanLast10()
{
    std::size_t N = 10;
    const auto records = getMeasurements(N);

    auto accumulators = std::make_tuple(0.0, 0.0, 0.0);

    std::for_each(records.begin(), records.end(), [&](const Data& d) {
        std::get<0>(accumulators) += d.pressure;
        std::get<1>(accumulators) += d.temperature;
        std::get<2>(accumulators) += d.velocity;
    });

    auto [sumPressure, sumTemperature, sumVelocity] = accumulators;
    double meanPressure = sumPressure / static_cast<double>(N);
    double meanTemperature = sumTemperature / static_cast<double>(N);
    double meanVelocity = sumVelocity / static_cast<double>(N);

    return {.pressure = meanPressure, .temperature = meanTemperature, .velocity = meanVelocity};
}

void Sender::addData(const Data& data)
{
    if (dataSet.insert(data).second) {
        database.insertRecordToMessages(data);
    }
}
