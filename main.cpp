#include <QCoreApplication>

#include <QHttpServer>
#include <QHttpServerRequest>
#include <QHttpServerResponder>
#include <QHttpServerResponse>
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

  httpServer.route("/start", [&]() {
    auto response = sender.startSending();

    switch (response) {
    case QSerialPort::SerialPortError::NoError:
      return QHttpServerResponder::StatusCode::Ok;
    case QSerialPort::SerialPortError::TimeoutError:
      return QHttpServerResponder::StatusCode::InternalServerError;
    case QSerialPort::SerialPortError::UnknownError:
      return QHttpServerResponder::StatusCode::InternalServerError;
    }
    return QHttpServerResponder::StatusCode::Ok;

  });

  httpServer.route("/stop", [&]() {
    auto response = sender.stopSending();
    switch (response) {
    case QSerialPort::SerialPortError::NoError:
      return QHttpServerResponder::StatusCode::Ok;
    case QSerialPort::SerialPortError::TimeoutError:
      return QHttpServerResponder::StatusCode::InternalServerError;
    case QSerialPort::SerialPortError::UnknownError:
      return QHttpServerResponder::StatusCode::InternalServerError;
    }
    return QHttpServerResponder::StatusCode::Ok;

  });

  httpServer.route("/configure", [](const QHttpServerRequest &request) {
    const auto method = request.method();
    if (method == QHttpServerRequest::Method::Put) {
      auto requestData = request.body();
      QUrlQuery query(requestData);

      int frequency = query.queryItemValue("frequency").toInt();
      int debug = query.queryItemValue("debug").toInt();

      qDebug() << "Received integers:" << frequency << debug;
    }
    return host(request) + u"/query/"_s;
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
  //  // Start the HTTP server on localhost at port 7100
  //  if (httpServer.listen(QHostAddress::LocalHost, 7100)) {
  //    qDebug() << "HTTP server listening on localhost port 7100...";
  //  } else {
  //    qCritical() << "Failed to start HTTP server!";
  //    return 1;
  //  }

  return app.exec();
}