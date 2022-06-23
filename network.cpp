#include "network.h"



network::network(QObject *parent) : QObject(parent)
{

}

void network::connect_(tcp_role_t role, QString addr, QString port)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &network::readMessage);
    socket->connectToHost(addr,port.toInt());
}

void network::readMessage()
{
    message = socket->readAll();
    qDebug() << message;
}

