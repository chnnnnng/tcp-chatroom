#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

enum tcp_role_t {
    tcp_role_server,
    tcp_role_client
};


class network : public QObject
{
    Q_OBJECT
public:
    network(QObject *parent = 0);
    ~network();
    void connect_(tcp_role_t role, QString addr, QString port);
    void send();
    void disconnect();

private:
    QTcpSocket *socket;
    QString message;
private slots:
    void readMessage();
};

#endif // NETWORK_H
