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