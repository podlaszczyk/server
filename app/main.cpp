#include "LogMessageHandler.h"
#include "Server.h"

#include <QCommandLineParser>
#include <QCoreApplication>

bool parserCLI(const QCoreApplication& app, UARTParameters& uartParameters, HTTPParameters& httpParameters)
{
    QCommandLineParser parser;
    parser.addHelpOption();

    QCommandLineOption uartPortOption(QStringList() << "port",
                                      "Set serial port name (/dev/ttyUSB0).",
                                      "port",
                                      "/dev/ttyUSB0");

    QCommandLineOption baudRateOption(QStringList() << "baudrate",
                                      "Set baud rate (e.g., 9600, 115200).",
                                      "baudrate",
                                      "9600");

    QCommandLineOption hostOption(QStringList() << "host", "Set host address (e.g 127.0.0.1) ", "host", "localhost");

    QCommandLineOption httpPortOption(QStringList() << "httpPort", "Set httpPort (e.g 7100) ", "httpPort", "7100");

    QCommandLineOption dbOption(QStringList() << "db", "Set db (e.g database.db) ", "db", "database.db");
    QCommandLineOption verboseOption({
        {"v", "verbose"},
        "Enable verbose output to console"
    });

    parser.addOption(verboseOption);
    parser.addOption(uartPortOption);
    parser.addOption(baudRateOption);
    parser.addOption(hostOption);
    parser.addOption(httpPortOption);
    parser.addOption(dbOption);

    parser.process(app);

    bool verbose = parser.isSet("verbose");
    if (!verbose) {
        qInstallMessageHandler(LogMessageHandler);
    }
    if (parser.isSet("help")) {
        parser.showHelp(0);
    }

    QString portName = parser.value(uartPortOption);
    auto baudRateString = parser.value(baudRateOption);

    bool conversionOk = false;
    int baudRate = baudRateString.toInt(&conversionOk);
    if (!conversionOk) {
        qWarning() << "baudrate must be number";
        return 0;
    }

    QRegularExpression portRegex("^/dev/tty[A-Za-z0-9]+$");
    if (!portRegex.match(portName).hasMatch()) {
        qWarning() << "Invalid port name. Port must start with '/dev/tty' followed by alphanumeric characters.";
        parser.showHelp();
    }
    uartParameters.port = portName.toStdString();
    uartParameters.baudrate = baudRate;

    QString hostName = parser.value(hostOption);
    int httpPort = parser.value(httpPortOption).toInt();
    auto db = parser.value(dbOption);

    httpParameters.host = hostName.toStdString();
    httpParameters.port = httpPort;
    httpParameters.database_path = db.toStdString();

    qDebug() << "Serial port: " << portName;
    qDebug() << "Baud rate: " << baudRate;
    qDebug() << "Host: " << hostName;
    qDebug() << "Http port: " << httpPort;
    qDebug() << "DB " << db;
    return 1;
}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    UARTParameters uartParameters;
    HTTPParameters httpParameters;
    if (!parserCLI(app, uartParameters, httpParameters)) {
        return -1;
    }
    QString basePath = QCoreApplication::applicationDirPath();
    qDebug() << "Base Path:" << basePath;
    Server server(uartParameters, httpParameters);
    if (!server.getUartStatus()) {
        qWarning() << "MAIN: Application failed due to error with UART. Check logs";
        return -1;
    }
    server.startHttpListening();

    return app.exec();
}
