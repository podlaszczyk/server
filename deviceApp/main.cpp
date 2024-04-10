#include <LogMessageHandler.h>

#include "device/Device.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    qInstallMessageHandler(LogMessageHandler);

    QSerialPort serialPort;
    Device device;
    device.openUARTConnection();

    return app.exec();
}
