#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextCursor>
#include <QDateTime>
#include <chngnetwork.h>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_server_clicked();
    void on_btn_client_clicked();
    void on_btn_connect_clicked();
    void on_btn_send_clicked();

    void on_message_arrive();
    void on_connection_established();
    void on_connection_failed(QString err);
    void on_connection_closed();
    void on_server_listen_start();
    void on_server_stop();
private:
    Ui::MainWindow *ui;
    tcp_role_t role = tcp_role_server;
    std::unique_ptr<ChngNetwork> net;
};
#endif // MAINWINDOW_H
