#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QNetworkAccessManager *netManager;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString getUserName();
    QString getGameId();
    void displayBoard(QVector<QVector<int>>board);
    void clearBoard();

private slots:
    void CreateRoomPressed();
    void JoinRoomPressed();
};
#endif // MAINWINDOW_H
