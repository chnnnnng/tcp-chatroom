#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->btn_server->setEnabled(false);
    ui->te_dialog->setReadOnly(true);
    ui->btn_send->setEnabled(false);
    ui->btn_connect->setText("Listen");
    this->setWindowTitle("A demo for TCP chatroom | CHNG");
    net.reset(new ChngNetwork());

    connect(ui->le_msg, &QLineEdit::returnPressed, ui->btn_send, &QAbstractButton::click, Qt::UniqueConnection);
    connect(net.get(), &ChngNetwork::onMessage, this, &MainWindow::on_message_arrive);
    connect(net.get(), &ChngNetwork::onConnectionEstablished, this, &MainWindow::on_connection_established);
    connect(net.get(), &ChngNetwork::onConnectionError, this, &MainWindow::on_connection_failed);
    connect(net.get(), &ChngNetwork::onConnectionClosed, this, &MainWindow::on_connection_closed);
    connect(net.get(), &ChngNetwork::onServerListenStart, this, &MainWindow::on_server_listen_start);
    connect(net.get(), &ChngNetwork::onServerStop, this, &MainWindow::on_server_stop);
}

MainWindow::~MainWindow()
{
    delete ui;
    net.release();
}


void MainWindow::on_btn_server_clicked()
{
    if(net->connected) return;
    qDebug() << "server mode selected";
    ui->btn_server->setEnabled(false);
    ui->btn_client->setEnabled(true);
    ui->btn_connect->setText("Listen");
    role = tcp_role_server;
}


void MainWindow::on_btn_client_clicked()
{
    if(net->connected) return;
    qDebug() << "client mode selected";
    ui->btn_server->setEnabled(true);
    ui->btn_client->setEnabled(false);
    ui->btn_connect->setText("Connect");
    role = tcp_role_client;
}


void MainWindow::on_btn_connect_clicked()
{
    if(! net->connected && !net->listening){//如果未连接，此按键功能为连接

        net->init(role, ui->le_addr->text(), ui->le_port->text());
    }else{//如果已连接，此按键功能为断开连接
        net->disconnect();
        //net.reset(new ChngNetwork());
    }
}

void MainWindow::on_message_arrive()
{
    QTextCharFormat fmt;
    fmt.setForeground(QBrush(QColor(("#845EC2"))));
    ui->te_dialog->mergeCurrentCharFormat(fmt);         //设置字体颜色
    QDateTime curDateTime = QDateTime::currentDateTime();
    ui->te_dialog->appendPlainText(curDateTime.toString(">HH:mm:ss.zzz\t") + net->getMessage());   //追加一行
    ui->te_dialog->moveCursor(QTextCursor::End);    //光标移到最后
}

void MainWindow::on_connection_established()
{
    ui->btn_connect->setText("Disconnect");
    ui->btn_send->setEnabled(true);
}

void MainWindow::on_connection_failed(QString err)
{
    QMessageBox::critical(this, "CHNG Chatroom", "连接时发生错误，错误信息："+err);
}

void MainWindow::on_connection_closed()
{
    if(role == tcp_role_client) ui->btn_connect->setText("Connect");
    else ui->btn_connect->setText("Listen");
    ui->btn_send->setEnabled(false);
}

void MainWindow::on_server_listen_start()
{
    ui->btn_connect->setText("Listening");
}

void MainWindow::on_server_stop()
{
    ui->btn_connect->setText("Listen");
}


void MainWindow::on_btn_send_clicked()
{
    QString msg = ui->le_msg->text();
    if(msg.isEmpty()) return;
    qDebug() << "send: " << msg;
    net->send(msg);         //发送
    ui->le_msg->clear();    //清空发送区

    QTextCharFormat fmt;
    fmt.setForeground(QBrush(QColor(("#FF6F91"))));
    ui->te_dialog->mergeCurrentCharFormat(fmt);     //设置字体颜色
    QDateTime curDateTime = QDateTime::currentDateTime();
    ui->te_dialog->appendPlainText( curDateTime.toString("<HH:mm:ss.zzz\t") + msg);        //追加一行
    ui->te_dialog->moveCursor(QTextCursor::End);    //光标移到最后
}

