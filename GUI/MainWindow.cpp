#include "MainWindow.h"

#include "CloneWidget.h"

#include <QFile>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPainter>
#include <QPlainTextEdit>
#include <QPushButton>
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

    frequencyLineEdit = new QLineEdit(this);
    frequencyLineEdit->setPlaceholderText("frequency");

    debugLineEdit = new QLineEdit(this);
    debugLineEdit->setPlaceholderText("debug");

    auto putButton = new QPushButton("Send Config", this);
    connect(putButton, &QPushButton::clicked, this, &MainWindow::onConfigurationClicked);

    limitLineEdit = new QLineEdit(this);
    limitLineEdit->setPlaceholderText("limit of messages");

    auto limitButton = new QPushButton("Limit messages", this);
    connect(limitButton, &QPushButton::clicked, this, &MainWindow::onMessagesLimitClicked);

    responseTextEdit = new QTextEdit(this);
    responseTextEdit->setReadOnly(true);

    logServerPlainTextEdit = new QPlainTextEdit(this);
    logServerPlainTextEdit->setReadOnly(true);

    logDevicePlainTextEdit = new QPlainTextEdit(this);
    logDevicePlainTextEdit->setReadOnly(true);

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
    mainLayout->addWidget(cloneWidget, 0, Qt::AlignCenter);
    mainLayout->addWidget(serverUrlLineEdit);
    mainLayout->addWidget(startButton);
    mainLayout->addWidget(stopButton);
    mainLayout->addWidget(deviceButton);
    mainLayout->addWidget(frequencyLineEdit);
    mainLayout->addWidget(debugLineEdit);
    mainLayout->addWidget(putButton);
    mainLayout->addWidget(limitLineEdit);
    mainLayout->addWidget(limitButton);
    mainLayout->addWidget(responseTextEdit);
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
    postData.addQueryItem("frequency", frequencyLineEdit->text());
    postData.addQueryItem("debug", debugLineEdit->text());
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
        QMessageBox::information(this, "Information", "OK");
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
