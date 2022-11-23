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
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            QString index = QString::number(3 * i + j);
            QPushButton *playButton = findChild<QPushButton *>("pushButton_" + index);
            connect(playButton, SIGNAL(released()), this, SLOT(GamePlayPressed()));

        }
    }
    clearBoard();
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
        // TODO: clear board and disconnect from socket and enemy name
        return;
    }
    QNetworkRequest request = QNetworkRequest(QUrl("http://localhost:8080/game/join"));
    /*{
        "player": {
            "login": "isaac2"
        },
        "gameId": "dbac21ab-0d04-4036-90bb-1968a419f70c"
    }*/
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
/*
            QLabel *infoLabel = findChild<QLabel *>("infoLabel");
            infoLabel->setText("Playing with " + jsonObject["player1"].toObject()["login"].toString());
            QJsonArray arr = jsonObject["board"].toArray();
            qDebug() << arr;
            QVector<QVector<int>> board;

            displayBoard(board);
*/
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
    return;
}
