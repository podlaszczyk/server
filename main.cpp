#include <QCoreApplication>

#include <QString>
#include <QTimer>

#include "Device.h"

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  Device reader;

  QObject::connect(&reader, &Device::newData, [&](const QByteArray &data) {
    qDebug() << "Received data:" << data;
  });

  return app.exec();
}