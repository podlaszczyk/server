#pragma once

#include <Data.h>
#include <Database.h>

#include <QSerialPort>
#include <QtCore>
#include <QTimer>

#include <deque>
#include <unordered_set>

class Sender : public QObject
{
    Q_OBJECT

public:
    Sender(const UARTParameters& uartParameters, const Database& db, QObject* parent = nullptr);

    void sendRequest(const QByteArray& request);

    Config getConfig();
    std::deque<Data> getMeasurements(int number);
    Data getLatest();
    Data getMeanLast10();

signals:
    void requestResult(const QString& result);

private:
    QSerialPort serial;
    int timeout = 3000;
    QTimer timer;

    void handleError(QSerialPort::SerialPortError error);
    void handleReadyRead();
    void handleTimeout();
    void processMessage(const QByteArray& message);

    void addData(const Data& data);

    std::unordered_set<Data, Data::Hash> dataSet;
    Database database;
};
