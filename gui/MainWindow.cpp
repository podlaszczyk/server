#include "MainWindow.h"

#include "CloneWidget.h"

#include <QFile>
#include <QFileDialog>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPainter>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QTimer>
#include <QUrlQuery>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("GUI Application");

    cloneWidget = new CloneWidget(this);

    networkManager = new QNetworkAccessManager(this);

    serverUrlLineEdit = new QLineEdit(this);
    serverUrlLineEdit->setPlaceholderText("Enter server URL( http://localhost:7100 )");
    serverUrlLineEdit->setText("http://localhost:7100");

    auto startButton = new QPushButton("Start", this);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartClicked);

    auto stopButton = new QPushButton("Stop", this);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::onStopClicked);

    auto deviceButton = new QPushButton("Device", this);
    connect(deviceButton, &QPushButton::clicked, this, &MainWindow::onDeviceClicked);

    //    frequencyLineEdit = new QLineEdit(this);
    //    frequencyLineEdit->setPlaceholderText("frequency");
    //    QIntValidator* validator = new QIntValidator(1, 100, frequencyLineEdit);
    //    frequencyLineEdit->setValidator(validator);
    frequencySpinBox = new QSpinBox;
    frequencySpinBox->setRange(1, 100);

    debugLineEdit = new QLineEdit(this);
    debugLineEdit->setPlaceholderText("debug");

    auto putButton = new QPushButton("Send Config", this);
    connect(putButton, &QPushButton::clicked, this, &MainWindow::onConfigurationClicked);

    auto configLayout = new QHBoxLayout;
    auto freqAndDebugLayout = new QVBoxLayout;
    freqAndDebugLayout->addWidget(frequencySpinBox);
    freqAndDebugLayout->addWidget(debugLineEdit);
    configLayout->addWidget(putButton);
    configLayout->addLayout(freqAndDebugLayout);

    limitLineEdit = new QLineEdit(this);
    limitLineEdit->setPlaceholderText("limit of messages");

    auto limitButton = new QPushButton("Limit messages", this);
    connect(limitButton, &QPushButton::clicked, this, &MainWindow::onMessagesLimitClicked);

    auto limitLayout = new QHBoxLayout;
    limitLayout->addWidget(limitButton);
    limitLayout->addWidget(limitLineEdit);

    responseTextEdit = new QTextEdit(this);
    responseTextEdit->setReadOnly(true);

    logServerPlainTextEdit = new QPlainTextEdit(this);
    logServerPlainTextEdit->setReadOnly(true);

    logDevicePlainTextEdit = new QPlainTextEdit(this);
    logDevicePlainTextEdit->setReadOnly(true);

    customMessageEdit = new QLineEdit(this);
    customMessageEdit->setPlaceholderText(
        "custom message e.g. ($0\\n), ($1,\\n), ($2,1,true\\n) or incorrect like ($0), ($3), ($2,0\\n)");

    auto sendMessageButton = new QPushButton("Send custom message", this);
    connect(sendMessageButton, &QPushButton::clicked, this, &MainWindow::onSendCustomMessageClicked);

    auto customMessageLayout = new QHBoxLayout;
    customMessageLayout->addWidget(sendMessageButton);
    customMessageLayout->addWidget(customMessageEdit);

    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &MainWindow::readLogFile);
    refreshTimer->start(1000);

    auto loadAppLogButton = new QPushButton("Load App Log (Bin CloneApp Directory)", this);
    connect(loadAppLogButton, &QPushButton::clicked, this, &MainWindow::onLoadAppLog);

    auto loadDeviceLogButton = new QPushButton("Load Device Log (Bin DeviceApp Directory)", this);
    connect(loadDeviceLogButton, &QPushButton::clicked, this, &MainWindow::onLoadDeviceLog);

    auto logAppLayout = new QVBoxLayout;
    logAppLayout->addWidget(loadAppLogButton);
    logAppLayout->addWidget(logServerPlainTextEdit);

    auto logDeviceLayout = new QVBoxLayout;
    logDeviceLayout->addWidget(loadDeviceLogButton);
    logDeviceLayout->addWidget(logDevicePlainTextEdit);

    auto logsLayout = new QHBoxLayout;
    logsLayout->addLayout(logAppLayout);
    logsLayout->addLayout(logDeviceLayout);

    auto mainLayout = new QVBoxLayout;

    auto requestsLayout = new QVBoxLayout;

    auto requestAndOutputLayout = new QHBoxLayout;

    requestsLayout->addWidget(serverUrlLineEdit);
    requestsLayout->addWidget(startButton);
    requestsLayout->addWidget(stopButton);
    requestsLayout->addWidget(deviceButton);
    requestsLayout->addLayout(configLayout);
    requestsLayout->addLayout(limitLayout);
    requestsLayout->addLayout(customMessageLayout);

    requestAndOutputLayout->addLayout(requestsLayout);
    requestAndOutputLayout->addWidget(responseTextEdit);

    mainLayout->addWidget(cloneWidget, 0, Qt::AlignCenter);
    mainLayout->addLayout(requestAndOutputLayout);
    mainLayout->addLayout(logsLayout, 1);

    auto centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow()
{
    delete networkManager;
}

void MainWindow::onStartClicked()
{
    QString urlStr = serverUrlLineEdit->text() + "/start";
    QNetworkRequest request(urlStr);

    QNetworkReply* reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, [&, reply]() {
        handleNetworkReply(reply);
    });
}

void MainWindow::onStopClicked()
{
    QString urlStr = serverUrlLineEdit->text() + "/stop";
    QNetworkRequest request(urlStr);

    QNetworkReply* reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleNetworkReply(reply);
    });
}

void MainWindow::onDeviceClicked()
{
    QString urlStr = serverUrlLineEdit->text() + "/device";
    QNetworkRequest request(urlStr);

    QNetworkReply* reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleNetworkReply(reply);
    });
}

void MainWindow::onConfigurationClicked()
{
    QString urlStr = serverUrlLineEdit->text() + "/configure";
    QUrl url(urlStr);

    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    QUrlQuery postData;
    postData.addQueryItem("frequency", frequencySpinBox->text());
    postData.addQueryItem("debug", debugLineEdit->text());
    QByteArray postDataByteArray = postData.toString(QUrl::FullyEncoded).toUtf8();

    QByteArray requestData;
    QNetworkReply* reply = networkManager->put(request, postDataByteArray);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleNetworkReply(reply);
    });
}

void MainWindow::onSendCustomMessageClicked()
{
    QString urlStr = serverUrlLineEdit->text() + "/customMessage";
    QUrl url(urlStr);

    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    QUrlQuery postData;
    auto text = customMessageEdit->text();
    postData.addQueryItem("message", text);
    QByteArray postDataByteArray = postData.toString(QUrl::FullyEncoded).toUtf8();

    QByteArray requestData;
    QNetworkReply* reply = networkManager->put(request, postDataByteArray);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleNetworkReply(reply);
    });
}

void MainWindow::onMessagesLimitClicked()
{
    const auto limit = limitLineEdit->text();

    QString urlStr = serverUrlLineEdit->text() + "/messages?limit=" + limit;
    QNetworkRequest request(urlStr);

    QNetworkReply* reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleNetworkReply(reply);
    });
}

void MainWindow::handleNetworkReply(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QMessageBox::information(this, "Information", "Succesfull operation");
        responseTextEdit->setText(reply->readAll());
    }
    else {
        QMessageBox::warning(this, "Error", "HTTP request failed: " + reply->errorString());
    }
    reply->deleteLater();
}

void MainWindow::readLogFile()
{
    if (!logServerFilePath.isEmpty()) {
        QFile logServerFile(logServerFilePath);
        if (logServerFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&logServerFile);
            logServerPlainTextEdit->setPlainText(in.readAll());
            QTextCursor cursor = logServerPlainTextEdit->textCursor();
            cursor.movePosition(QTextCursor::End);
            logServerPlainTextEdit->setTextCursor(cursor);

            logServerFile.close();
        }
    }
    if (!logDeviceFilePath.isEmpty()) {
        QFile logDeviceFile(logDeviceFilePath);
        if (logDeviceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&logDeviceFile);
            logDevicePlainTextEdit->setPlainText(in.readAll());
            QTextCursor cursor = logDevicePlainTextEdit->textCursor();
            cursor.movePosition(QTextCursor::End);
            logDevicePlainTextEdit->setTextCursor(cursor);

            logDeviceFile.close();
        }
    }
}

void MainWindow::onLoadAppLog()
{
    QString currentDir = QDir::currentPath();
    QDir dir(currentDir);
    dir.cdUp();
    const auto defaultDir = dir.path();

    QFileDialog::Options options;
    options |= QFileDialog::DontUseNativeDialog;

    QString fileName =
        QFileDialog::getOpenFileName(nullptr, "Open File", defaultDir, "Text Files (*.log)", nullptr, options);

    if (fileName.isEmpty()) {
        return;
    }

    logServerFilePath = fileName;
}

void MainWindow::onLoadDeviceLog()
{
    QString currentDir = QDir::currentPath();
    QDir dir(currentDir);
    dir.cdUp();
    const auto defaultDir = dir.path();

    QFileDialog::Options options;
    options |= QFileDialog::DontUseNativeDialog;

    QString fileName =
        QFileDialog::getOpenFileName(nullptr, "Open File", defaultDir, "Text Files (*.log)", nullptr, options);

    if (fileName.isEmpty()) {
        return;
    }

    logDeviceFilePath = fileName;
}
