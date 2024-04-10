#pragma once

#include <QMainWindow>

class QNetworkAccessManager;
class QLineEdit;
class QTextEdit;
class QNetworkReply;
class QPlainTextEdit;
class QLabel;
class CloneWidget;
class QSpinBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() final;

private:
    void onStartClicked();
    void onStopClicked();
    void onConfigurationClicked();
    void onMessagesLimitClicked();
    void handleNetworkReply(QNetworkReply* reply);
    void onLoadAppLog();
    void onLoadDeviceLog();
    void onDeviceClicked();
    void onSendCustomMessageClicked();

private:
    QNetworkAccessManager* networkManager;
    QLineEdit* serverUrlLineEdit;
    //    QLineEdit* frequencyLineEdit = nullptr;
    QLineEdit* customMessageEdit;
    QLineEdit* debugLineEdit;
    QLineEdit* limitLineEdit;
    QTextEdit* responseTextEdit;
    QPlainTextEdit* logServerPlainTextEdit;
    QPlainTextEdit* logDevicePlainTextEdit;
    QSpinBox* frequencySpinBox;

    CloneWidget* cloneWidget;

    QTimer* refreshTimer;
    QString logServerFilePath;
    QString logDeviceFilePath;
    void readLogFile();
};
