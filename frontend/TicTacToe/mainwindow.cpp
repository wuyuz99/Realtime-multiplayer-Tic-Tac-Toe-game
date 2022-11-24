#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QByteArray>
#include <QNetworkReply>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    netManager = new QNetworkAccessManager(this);
    QPushButton *hostButton = MainWindow::findChild<QPushButton *>("hostButton");
    QPushButton *joinButton = MainWindow::findChild<QPushButton *>("joinButton");
    connect(hostButton, SIGNAL(released()), this, SLOT(CreateRoomPressed()));
    connect(joinButton, SIGNAL(released()), this, SLOT(JoinRoomPressed()));
    connect(&m_webSocket, &QWebSocket::connected, this, &MainWindow::SocketConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &MainWindow::SocketClosed);
    m_webSocket.open(QUrl(QStringLiteral("ws://localhost:8080/gameplay")));
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            QString index = QString::number(3 * i + j);
            QPushButton *playButton = findChild<QPushButton *>("pushButton_" + index);
            connect(playButton, SIGNAL(released()), this, SLOT(GamePlayPressed()));

        }
    }
    clearBoard();
    playerTurn = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getUserName() {
    QLineEdit *nameEdit = this->findChild<QLineEdit *>("nameEdit");
    return nameEdit->text();
}

QString MainWindow::getGameId() {
    QLineEdit *roomEdit = this->findChild<QLineEdit *>("roomEdit");
    return roomEdit->text();
}

void MainWindow::CreateRoomPressed() {
    QNetworkRequest request = QNetworkRequest(QUrl("http://localhost:8080/game/host"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject obj;
    obj["login"] = getUserName();
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    QNetworkReply *reply = netManager->post(request, data);
    MainWindow::connect(reply, &QNetworkReply::finished, [=](){
        if(reply->error() == QNetworkReply::NoError){
            QString strReply = QString::fromUtf8(reply->readAll());
            QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
            QJsonObject jsonObject = jsonResponse.object();

            QLineEdit *roomEdit = findChild<QLineEdit *>("roomEdit");
            roomEdit->setText(jsonObject["gameId"].toString());
            roomEdit->setReadOnly(true);

            QPushButton *joinButton = MainWindow::findChild<QPushButton *>("joinButton");
            joinButton->setText("Clear");
            qDebug() << jsonObject;
            clearBoard();
            gameId = jsonObject["gameId"].toString();
            playerType = "X";
            playerTurn = true;

        }
        else{
            QString err = reply->errorString();
            qDebug() << err;
        }
        reply->deleteLater();
    });
}

void MainWindow::JoinRoomPressed() {
    QPushButton *button = (QPushButton *) sender();
    QString operation = button->text();
    if (operation == "Clear") {
        QLineEdit *roomEdit = findChild<QLineEdit *>("roomEdit");
        roomEdit->setText("");
        roomEdit->setReadOnly(false);
        button->setText("Join");
        clearBoard();
        // TODO: clear board and disconnect from socket and enemy name
        return;
    }
    QNetworkRequest request = QNetworkRequest(QUrl("http://localhost:8080/game/join"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject obj;
    QJsonObject player;
    player["login"] = getUserName();
    obj["player"] = player;
    obj["gameId"] = getGameId();
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    qDebug() << data;
    QNetworkReply* reply = netManager->post(request, data);
    MainWindow::connect(reply, &QNetworkReply::finished, [=](){
        if(reply->error() == QNetworkReply::NoError){
            QString strReply = QString::fromUtf8(reply->readAll());
            QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
            QJsonObject jsonObject = jsonResponse.object();

            QLineEdit *roomEdit = findChild<QLineEdit *>("roomEdit");
            roomEdit->setText(jsonObject["gameId"].toString());
            roomEdit->setReadOnly(true);

            QPushButton *joinButton = MainWindow::findChild<QPushButton *>("joinButton");
            joinButton->setText("Clear");
            qDebug() << jsonObject;

            gameId = jsonObject["gameId"].toString();
            playerType = "O";
            clearBoard();
            // set up connection for web socket
        }
        else{
            QString err = reply->errorString();
            qDebug() << err;
        }
        reply->deleteLater();
    });
}

void MainWindow::displayBoard(QVector<QVector<int>> board) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            QString index = QString::number(3 * i + j);
            QPushButton *button = findChild<QPushButton *>("pushButton_" + index);
            if (board[i][j] == 0) {
                button->setText("");
                button->setDisabled(false);
            } else if (board[i][j] == 1) {
                button->setText("O");
                button->setDisabled(true);
            } else if (board[i][j] == 2) {
                button->setText("X");
                button->setDisabled(true);
            }
        }
    }
}

void MainWindow::clearBoard() {
    QVector<QVector<int>> board;
    for (int i = 0; i < 3; ++i) {
        board.append(QVector<int> {0, 0, 0});
    }
    qDebug() << board;
    displayBoard(board);
}

void MainWindow::GamePlayPressed() {
    if (!playerTurn) {
        return;
    }
    QPushButton *button = (QPushButton *) sender();
    int x, y;
    QString button_name = button->objectName();
    int index = button_name.last(1).toInt();
    x = index / 3;
    y = index % 3;
    QNetworkRequest request = QNetworkRequest(QUrl("http://localhost:8080/game/gameplay"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject obj;
    obj["type"] = playerType;
    obj["coordX"] = x;
    obj["coordY"] = y;
    obj["gameId"] = gameId;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    QNetworkReply* reply = netManager->post(request, data);
    MainWindow::connect(reply, &QNetworkReply::finished, [=](){
        if(reply->error() == QNetworkReply::NoError){
            QString strReply = QString::fromUtf8(reply->readAll());
            QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
            QJsonObject jsonObject = jsonResponse.object();
            qDebug() << jsonObject;
        }
        else{
            QString err = reply->errorString();
            qDebug() << err;
        }
        reply->deleteLater();
    });
    playerTurn = false;
}


void MainWindow::SocketConnected() {
    qDebug() << "web socket connected";
}

void MainWindow::SocketMsgRecved(QString message) {
    qDebug() << "message received" << message;
}

void MainWindow::SocketClosed() {
    qDebug() << "web socket closed";
}
