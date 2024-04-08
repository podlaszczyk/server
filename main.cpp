#include <QCoreApplication>

#include <QHttpServer>
#include <QHttpServerRequest>
#include <QHttpServerResponder>
#include <QString>

#include "Sender.h"

using namespace Qt::StringLiterals;

static inline QString host(const QHttpServerRequest &request) {
  return QString::fromLatin1(request.value("Host"));
}

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  Sender sender;

  QHttpServer httpServer;
  httpServer.route("/", [&]() { return "Hello world"; });

  QString requestResult;
  QEventLoop loop;

  QObject::connect(&sender, &Sender::requestResult, [&](const QString &result) {
    requestResult = result;
    qDebug() << "Request result:" << requestResult;
    loop.quit();
  });

  httpServer.route("/start", [&]() {
    sender.sendRequest("$0\n");
    loop.exec();

    if (requestResult == "ok") {
      return QHttpServerResponder::StatusCode::Ok;
    }
    return QHttpServerResponder::StatusCode::InternalServerError;
  });

  httpServer.route("/stop", [&]() {
    sender.sendRequest("$1\n");
    loop.exec();

    if (requestResult == "ok") {
      return QHttpServerResponder::StatusCode::Ok;
    }
    return QHttpServerResponder::StatusCode::InternalServerError;
  });

  httpServer.route("/configure", [&](const QHttpServerRequest &request) {
    const auto method = request.method();
    if (method == QHttpServerRequest::Method::Put) {
      auto requestData = request.body();
      QUrlQuery query(requestData);

      const auto frequency = query.queryItemValue("frequency");
      const auto debug = query.queryItemValue("debug");

      qDebug() << "Received Http Values:" << frequency << debug;

      const QString uartReq = QString("$2,") + frequency + "," + debug + "\n";

      sender.sendRequest(uartReq.toLocal8Bit());
      loop.exec();

      if (requestResult == "ok") {
        return QHttpServerResponder::StatusCode::Ok;
      }
    }
    return QHttpServerResponder::StatusCode::InternalServerError;
  });

  httpServer.route("/messages", [&](const QHttpServerRequest &request) {
    QUrl url = request.url();
    QUrlQuery query(url.query());

    if (query.hasQueryItem("limit")) {
      QString limitValue = query.queryItemValue("limit");
      bool conversionOk;
      int limit = limitValue.toInt(&conversionOk);

      if (conversionOk && limit > 0) {
        QString responseMessage =
            QString("Retrieving messages with limit: %1").arg(limit);
        qDebug() << responseMessage;
        auto measurements = sender.getMeasurements(limit);

        QJsonArray jsonArray;
        for (const auto &data : measurements) {
          QJsonObject dataObject;
          dataObject["pressure"] = data.pressure;
          dataObject["temperature"] = data.temperature;
          dataObject["velocity"] = data.velocity;

          jsonArray.append(dataObject);
        }
        QJsonDocument jsonDocument(jsonArray);
        QByteArray jsonData = jsonDocument.toJson();

        return QHttpServerResponse(jsonData);

      } else {
        return QHttpServerResponse(
            QHttpServerResponder::StatusCode::BadRequest);
      }
    } else {
      return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);
    }
  });

  httpServer.route("/device", [&](const QHttpServerRequest &request) {
    const auto deviceConfig = sender.getConfig();
    const auto meanLast10 = sender.getMeanLast10();
    const auto latest = sender.getLatest();

    QJsonObject configJson;
    configJson["frequency"] = deviceConfig.frequency;
    configJson["debug"] = deviceConfig.debug;

    QJsonObject meanLast10Json;
    meanLast10Json["pressure"] = QString::number(meanLast10.pressure, 'f', 1).toDouble();
    meanLast10Json["temperature"] =
        QString::number(meanLast10.temperature, 'f', 1).toDouble();
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

  const auto port =
      httpServer.listen(QHostAddress::SpecialAddress::LocalHost, 7100);
  if (!port) {
    qWarning() << QCoreApplication::translate(
        "QHttpServerExample", "Server failed to listen on a port.");
    return -1;
  }
  qInfo().noquote() << QCoreApplication::translate(
                           "QHttpServerExample",
                           "Running on http://127.0.0.1:%1/"
                           "(Press CTRL+C to quit)")
                           .arg(port);

  return app.exec();
}