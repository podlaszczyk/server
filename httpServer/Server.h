#pragma once

#include "Sender.h"

#include <Database.h>

#include <QEventLoop>
#include <QHttpServer>
#include <QHttpServerRequest>
#include <QHttpServerResponder>

class Server : public QObject
{
    Q_OBJECT

public:
    Server(UARTParameters uartParameters, HTTPParameters httpParameters, QObject* parent = nullptr);

    bool getUartStatus() const;
    void startHttpListening();

private:
    UARTParameters uartParameters;

    HTTPParameters httpParameters;
    Database database;
    Sender sender;
    QHttpServer httpServer;
    QString requestResult;

    QEventLoop loop;

    bool senderStatus = false;
    void routes();
    void onReqResultReceived(const QString& result);
};
