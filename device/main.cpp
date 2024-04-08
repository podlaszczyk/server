#include "Device.h"

#include <LogMessageHandler.h>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    qInstallMessageHandler(LogMessageHandler);

    Device device;

    return app.exec();
}
