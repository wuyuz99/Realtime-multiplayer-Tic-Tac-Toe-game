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
    // connect signals
    QPushButton *hostButton = MainWindow::findChild<QPushButton *>("hostButton");
    QPushButton *joinButton = MainWindow::findChild<QPushButton *>("joinButton");
    connect(hostButton, SIGNAL(released()), this, SLOT(CreateRoomPressed()));
    connect(joinButton, SIGNAL(released()), this, SLOT(JoinRoomPressed()));
    connect(&m_webSocket, &QWebSocket::connected, this, &MainWindow::SocketConnected);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &MainWindow::SocketMsgRecved);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &MainWindow::SocketClosed);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            QString index = QString::number(3 * i + j);
            QPushButton *playButton = findChild<QPushButton *>("pushButton_" + index);
            connect(playButton, SIGNAL(released()), this, SLOT(GamePlayPressed()));

        }
    }
    // set board and turn
    clearBoard();
    playerTurn = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
 * helper functions
 */
QString MainWindow::getUserName() {
    QLineEdit *nameEdit = this->findChild<QLineEdit *>("nameEdit");
    return nameEdit->text();
}

QString MainWindow::getGameId() {
    QLineEdit *roomEdit = this->findChild<QLineEdit *>("roomEdit");
    return roomEdit->text();
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
                button->setText("X");
                button->setDisabled(true);
            } else if (board[i][j] == 2) {
                button->setText("O");
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
    displayBoard(board);
}

void MainWindow::prepareGame(QJsonObject jsonObject) {
    // set and lock edit
    QLineEdit *roomEdit = findChild<QLineEdit *>("roomEdit");
    roomEdit->setText(jsonObject["gameId"].toString());
    roomEdit->setReadOnly(true);
    QLineEdit *nameEdit = findChild<QLineEdit *>("nameEdit");
    nameEdit->setReadOnly(true);

    // make button clear
    QPushButton *joinButton = MainWindow::findChild<QPushButton *>("joinButton");
    joinButton->setText("Clear");

    // set member and clear board
    clearBoard();
    gameId = jsonObject["gameId"].toString();
    playerType = isHost ? "X": "O";
    // playerTurn = isHost;

    // connect to socket
    QString ws_uri = "ws://localhost:8080/gameplay/" + gameId;
    m_webSocket.open(QUrl(ws_uri));
    if (isHost)
        displayStatusMessage(gameId);
}

void MainWindow::displayStatusMessage(QString info) {
    qDebug() << "displaying info: " << info;
    QLabel *statusLabel = findChild<QLabel *>("statusLabel");
    statusLabel->setWordWrap(true);
    qDebug() << gameStatus;
    if(gameStatus == "NEW") {
        // info is the room number
        QString text = "Hosting game %1, waiting for opponent";
        text = text.arg(info);
        statusLabel->setText(text);
        return;
    }
    if(gameStatus == "STARTING" || gameStatus == "IN_PROGRESS") {
        // info is the opponent name
        QString text = "Opponent: %1";
        text = text.arg(info);
        if(playerTurn) {
            text.append(", your turn");
        } else {
            text.append(", waiting for opponent");
        }
        statusLabel->setText(text);
        return;
    }
    if(gameStatus == "FINISHED") {
        QString text = "GameOver, ";
        if (playerType == info) {
            text.append("you won");
        } else if (info == "") {
            text.append("draw");
        } else {
            text.append("you lost");
        }
        statusLabel->setText(text);
        return;
    }
}

void MainWindow::clearStatusMessage() {
    QLabel *statusLabel = findChild<QLabel *>("statusLabel");
    statusLabel->setText("");
}

/*
 * Button Callbacks with HTTP request sends
 */
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
            gameStatus = "NEW";
            isHost = true;
            prepareGame(jsonObject);
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

    // exit the room and clear the data
    if (operation == "Clear") {
        QLineEdit *roomEdit = findChild<QLineEdit *>("roomEdit");
        roomEdit->setText("");
        roomEdit->setReadOnly(false);
        QLineEdit *nameEdit = findChild<QLineEdit *>("nameEdit");
        nameEdit->setReadOnly(false);
        button->setText("Join");
        clearBoard();
        clearStatusMessage();
        m_webSocket.close();
        return;
    }

    // join a room
    QNetworkRequest request = QNetworkRequest(QUrl("http://localhost:8080/game/join"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject obj;
    QJsonObject player;
    player["login"] = getUserName();
    obj["player"] = player;
    obj["gameId"] = getGameId();
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    QNetworkReply* reply = netManager->post(request, data);
    MainWindow::connect(reply, &QNetworkReply::finished, [=](){
        if(reply->error() == QNetworkReply::NoError){
            QString strReply = QString::fromUtf8(reply->readAll());
            QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
            QJsonObject jsonObject = jsonResponse.object();
            isHost = false;
            prepareGame(jsonObject);
        }
        else{
            QString err = reply->errorString();
            qDebug() << err;
        }
        reply->deleteLater();
    });
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
        }
        else{
            QString err = reply->errorString();
            qDebug() << err;
        }
        reply->deleteLater();
    });
}

/*
 * socket callbacks to handle websocket messages
 */

void MainWindow::SocketConnected() {
    qDebug() << "web socket connected";
}

void MainWindow::SocketMsgRecved(QString message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    QString gameStatus = doc["status"].toString();
    this->gameStatus = gameStatus;
    QJsonArray arr = doc["board"].toArray();
    QVector<QVector<int>> board;
    for (int i = 0; i < 3; ++i) {
       QVector<int> line;
       QJsonArray originalLine =arr[i].toArray();
       for (int j = 0; j < 3; ++j) {
           int value = originalLine[j].toInt();
           line.append(value);
       }
       board.append(line);
    }
    displayBoard(board);
    if (gameStatus == "FINISHED") {
        QString winner = doc["winner"].toString();
        qDebug() << "winner is " << winner;
        displayStatusMessage(winner);
        playerTurn = false;
        return;
    }
    QString opponentField = isHost ? "player2" : "player1";
    QString opponent = doc[opponentField].toObject()["login"].toString();
    if (gameStatus == "IN_PROGRESS") {
        playerTurn = !playerTurn;
    }
    if (gameStatus == "STARTING") {
         playerTurn = isHost;
    }
    displayStatusMessage(opponent);

}

void MainWindow::SocketClosed() {
    qDebug() << "web socket closed";
}
