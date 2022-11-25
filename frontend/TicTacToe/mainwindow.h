#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QNetworkAccessManager>
#include <QWebSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QNetworkAccessManager *netManager;
    QWebSocket m_webSocket;
    QString playerType;
    QString gameId;
    bool playerTurn;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString getUserName();
    QString getGameId();
    void displayBoard(QVector<QVector<int>>board);
    void clearBoard();
    void prepareGame(QJsonObject jsonObject, bool isHost);
    void displayStatusMessage();

private slots:
    void CreateRoomPressed();
    void JoinRoomPressed();
    void GamePlayPressed();
    void SocketConnected();
    void SocketMsgRecved(QString message);
    void SocketClosed();
};
#endif // MAINWINDOW_H
