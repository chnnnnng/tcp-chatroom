#ifndef CHNGNETWORK_H
#define CHNGNETWORK_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QMessageBox>
#include <QMetaEnum>

enum tcp_role_t {
    tcp_role_server,
    tcp_role_client
};

class ChngNetwork : public QObject
{
    Q_OBJECT
public:
    explicit ChngNetwork(QObject *parent = nullptr);
    ~ChngNetwork();
    void init(tcp_role_t role, QString addr, QString port);
    void send(QString& msg);
    void disconnect();
    const QString &getMessage() const;
private:
    void startServer(QString addr, QString port);
    void stopServer();
public:
    bool connected = false;
    bool listening = false;
private:
    std::unique_ptr<QTcpSocket> socket;
    std::unique_ptr<QTcpServer> server;
    QString message;
    tcp_role_t role;
    QString addr;
    QString port;
signals:
    void onMessage();
    void onConnectionEstablished();
    void onConnectionError(QString err);
    void onConnectionClosed();
    void onServerListenStart();
    void onServerStop();
public slots:
    void onNewMessage();    //get new message (server or client)
    void onConnectSucceed();    //client connect to server suecceed (client)
    void onConnectFail(QAbstractSocket::SocketError err);   //connect fail (server or client)
    void onConnectionClose();   //connect close (server or client)
    void onNewConnection(); //server get new connection (client)
};

#endif // CHNGNETWORK_H
