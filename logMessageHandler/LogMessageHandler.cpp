#include "LogMessageHandler.h"

void LogMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QFile logFile("debug.log");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&logFile);
        stream << QDateTime::currentDateTime().toString(Qt::ISODate) << " ";
        stream << "[" << qPrintable(QCoreApplication::applicationName()) << "] ";
        switch (type) {
            case QtDebugMsg:
                stream << "DEBUG: ";
                break;
            case QtInfoMsg:
                stream << "INFO: ";
                break;
            case QtWarningMsg:
                stream << "WARNING: ";
                break;
            case QtCriticalMsg:
                stream << "CRITICAL: ";
                break;
            case QtFatalMsg:
                stream << "FATAL: ";
                break;
        }
        stream << msg << "\n";
        logFile.close();
    }
}
