#include "Server.h"

#include <LogMessageHandler.h>

#include <QCoreApplication>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    qInstallMessageHandler(LogMessageHandler);
    Server server;

    return app.exec();
}
