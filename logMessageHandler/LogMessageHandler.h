#pragma once

#include <QDebug>
#include <QFile>
#include <QtCore>
#include <QTextStream>

void LogMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);
