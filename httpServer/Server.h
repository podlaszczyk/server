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
    explicit Server(UARTParameters uartParameters, HTTPParameters httpParameters, QObject* parent = nullptr);

private:
    UARTParameters uartParameters;
    HTTPParameters httpParameters;

    Database database;
    Sender sender;
    QHttpServer httpServer;
    QString requestResult;
    QEventLoop loop;

    void routes();
    void startHttpListening();
    void stopLoopWhenReqResultReceived(const QString& result);
};
