#include "Sender.h"

#include <QThread>

Sender::Sender(const UARTParameters& uartParameters, const Database& db, QObject* parent)
    : QObject(parent)
    , database(db)
{
    const auto port = uartParameters.port.data();
    serial.setPortName(port);
    serial.setBaudRate(uartParameters.baudrate);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);

    connect(&serial, &QSerialPort::readyRead, this, &Sender::handleReadyRead);
    connect(&timer, &QTimer::timeout, this, &Sender::handleTimeout);
}

Sender::~Sender()
{
    serial.close();
}

void Sender::handleReadyRead()
{
    QByteArray data = serial.readAll();
    qDebug() << "SENDER: "
             << "Received data:" << data;

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
        if (message.startsWith("$0")) {
            if (message.startsWith("$0,ok")) {
                qDebug() << "SENDER: " << "Received start response:" << message;
                timer.stop();
                emit requestResult("ok");
            }
            if (message.startsWith("$0,invalid command")) {
                qWarning() << "SENDER: invalid command" << message;
                timer.stop();
                emit requestResult("invalid command");
            }
        }
        else if (message.startsWith("$1")) {
            if (message.startsWith("$1,ok")) {
                qDebug() << "SENDER: " << "Received stop response:" << message;
                timer.stop();
                emit requestResult("ok");
            }
            if (message.startsWith("$1,invalid command")) {
                timer.stop();
                qWarning() << "SENDER: invalid command" << message;
                emit requestResult("invalid command");
            }
        }

        else if (matchConfiguration.hasMatch()) {
            const auto frequency = matchConfiguration.captured(1).toInt();
            const bool debug = matchConfiguration.captured(2) == "true";
            const auto status = matchConfiguration.captured(3);

            database.insertRecordToConfiguration({.frequency = frequency, .debug = debug});

            qDebug() << "SENDER: "
                     << "Received config response:"
                     << "frequency" << frequency << "debug" << debug << "status" << status;
            timer.stop();
            emit requestResult(status);
        }
        else if (message.startsWith("$2,invalid command")) {
            timer.stop();
            qWarning() << "SENDER: invalid command" << message;
            emit requestResult("invalid command");
        }
        else if (matchMeasurement.hasMatch()) {
            const auto pressure = matchMeasurement.captured(1).toDouble();
            const auto temperature = matchMeasurement.captured(2).toDouble();
            const auto velocity = matchMeasurement.captured(3).toDouble();
            qDebug() << "SENDER: "
                     << "PRESSURE:" << pressure << "TEMPERATURE" << temperature << "VELOCITY" << velocity;

            addData({.pressure = pressure, .temperature = temperature, .velocity = velocity});
        }
    }
    else {
        qWarning() << "SENDER: Invalid message format:" << message;
    }
}

void Sender::handleTimeout()
{
    qWarning() << "SENDER: Request timed out";
    timer.stop();
    emit requestResult("timeout");
}

void Sender::sendRequest(const QByteArray& request)
{
    serial.write(request);
    timer.start(timeout);
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
    const auto records = getMeasurements(static_cast<int>(N));

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

bool Sender::openUARTConnection()
{
    if (serial.open(QIODevice::ReadWrite)) {
        qDebug() << "SENDER: "
                 << "Serial port opened successfully"
                 << "SERIAL PORT PARAMETERS: " << serial.portName() << serial.baudRate() << serial.dataBits()
                 << serial.parity() << serial.stopBits();
        return true;
    }

    qWarning() << "SENDER: "
               << "Failed to open serial port:" << serial.errorString()
               << "SERIAL PORT PARAMETERS: " << serial.portName() << serial.baudRate() << serial.dataBits()
               << serial.parity() << serial.stopBits();
    return false;
}
