#include "Server.h"

#include "Sender.h"

#include <QEventLoop>
#include <QHttpServer>
#include <QHttpServerRequest>
#include <QHttpServerResponder>

using namespace Qt::StringLiterals;

Server::Server(UARTParameters uartParameters, HTTPParameters httpParameters, QObject* parent)
    : QObject(parent)
    , uartParameters(std::move(uartParameters))
    , httpParameters(std::move(httpParameters))
    , database(this->httpParameters.database_path)
    , sender(this->uartParameters, database)
{
    database.countAndDisplayRecords();
    database.retrieveAndDisplayRecords();

    senderStatus = sender.openUARTConnection();

    QObject::connect(&sender, &Sender::requestResult, this, &Server::onReqResultReceived);
    routes();
}

void Server::routes()
{
    httpServer.route("/", [&]() {
        return "Clone Server";
    });

    httpServer.route("/start", [&]() {
        sender.sendRequest("$0\n");
        loop.exec();
        return handleUARTResponse();
    });

    httpServer.route("/stop", [&]() {
        sender.sendRequest("$1\n");
        loop.exec();
        return handleUARTResponse();
    });

    httpServer.route("/configure", [&](const QHttpServerRequest& request) {
        const auto method = request.method();
        if (method == QHttpServerRequest::Method::Put) {
            auto requestData = request.body();
            QUrlQuery query(requestData);

            const auto frequency = query.queryItemValue("frequency");
            const auto debug = query.queryItemValue("debug");

            qDebug() << "SERVER: " << "Received Http Values:" << frequency << debug;

            const QString uartReq = QString("$2,") + frequency + "," + debug + "\n";

            sender.sendRequest(uartReq.toLocal8Bit());
            loop.exec();
        }
        return handleUARTResponse();
    });

    httpServer.route("/messages", [&](const QHttpServerRequest& request) {
        QUrl url = request.url();
        QUrlQuery query(url.query());

        if (query.hasQueryItem("limit")) {
            QString limitValue = query.queryItemValue("limit");
            bool conversionOk;
            int limit = limitValue.toInt(&conversionOk);

            if (conversionOk && limit > 0) {
                QString responseMessage = QString("Retrieving messages with limit: %1").arg(limit);
                qDebug() << "SERVER:" << responseMessage;
                auto measurements = sender.getMeasurements(limit);

                QJsonArray jsonArray;
                for (const auto& data : measurements) {
                    QJsonObject dataObject;
                    dataObject["pressure"] = data.pressure;
                    dataObject["temperature"] = data.temperature;
                    dataObject["velocity"] = data.velocity;

                    jsonArray.append(dataObject);
                }
                QJsonDocument jsonDocument(jsonArray);
                QByteArray jsonData = jsonDocument.toJson();

                return QHttpServerResponse(jsonData);
            }
            else {
                return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);
            }
        }
        else {
            return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);
        }
    });

    httpServer.route("/device", [&](const QHttpServerRequest& request) {
        const auto deviceConfig = sender.getConfig();
        const auto meanLast10 = sender.getMeanLast10();
        const auto latest = sender.getLatest();

        QJsonObject configJson;
        configJson["frequency"] = deviceConfig.frequency;
        configJson["debug"] = deviceConfig.debug;

        QJsonObject meanLast10Json;
        meanLast10Json["pressure"] = QString::number(meanLast10.pressure, 'f', 1).toDouble();
        meanLast10Json["temperature"] = QString::number(meanLast10.temperature, 'f', 1).toDouble();
        meanLast10Json["velocity"] = QString::number(meanLast10.velocity, 'f', 1).toDouble();

        QJsonObject latestJson;
        latestJson["pressure"] = latest.pressure;
        latestJson["temperature"] = latest.temperature;
        latestJson["velocity"] = latest.velocity;

        QJsonObject deviceJson;
        deviceJson["curr_config"] = configJson;
        deviceJson["mean_last_10"] = meanLast10Json;
        deviceJson["latest"] = latestJson;

        QJsonDocument jsonDocument(deviceJson);
        QByteArray jsonData = jsonDocument.toJson();

        return QHttpServerResponse(jsonData);
    });

    httpServer.route("/customMessage", [&](const QHttpServerRequest& request) {
        const auto method = request.method();
        if (method == QHttpServerRequest::Method::Put) {
            auto requestData = request.body();
            QUrlQuery query(requestData);

            auto message = query.queryItemValue("message");
            message.replace("\\n", "\n");

            sender.sendRequest(message.toLocal8Bit());
            loop.exec();
        }
        return handleUARTResponse();
    });
}

void Server::startHttpListening()
{
    QHostAddress address(httpParameters.host.c_str());
    const auto port = httpServer.listen(address, httpParameters.port);
    if (!port) {
        qWarning() << "SERVER:" << QCoreApplication::translate("QHttpServer", "Server failed to listen on a port.")
                   << httpParameters.host << ":" << httpParameters.port;
        return;
    }
    qInfo() << "SERVER: "
            << QCoreApplication::translate("QHttpServer", "Http server running on http://%1:%2/")
                   .arg(httpParameters.host.c_str())
                   .arg(port);
}

void Server::onReqResultReceived(const QString& result)
{
    requestResult = result;
    qDebug() << "SERVER: Request result:" << requestResult;
    loop.quit();
}

bool Server::getUartStatus() const
{
    return senderStatus;
}

QHttpServerResponder::StatusCode Server::handleUARTResponse()
{
    if (requestResult == "ok") {
        return QHttpServerResponder::StatusCode::Ok;
    }
    if (requestResult == "invalid command") {
        return QHttpServerResponder::StatusCode::BadRequest;
    }
    if (requestResult == "timeout") {
        return QHttpServerResponder::StatusCode::RequestTimeout;
    }
    return QHttpServerResponder::StatusCode::InternalServerError;
}
