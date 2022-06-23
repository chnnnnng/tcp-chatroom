#include "chngnetwork.h"

ChngNetwork::ChngNetwork(QObject *parent)
    : QObject{parent}
{

}

ChngNetwork::~ChngNetwork()
{
    socket.release();
    server.release();
}


void ChngNetwork::init(tcp_role_t role, QString addr, QString port)
{
    this->role = role;
    if(role == tcp_role_client){
        socket.reset(new QTcpSocket(this));
        connect(socket.get(), &QTcpSocket::readyRead, this, &ChngNetwork::onNewMessage);
        connect(socket.get(), &QTcpSocket::connected, this, &ChngNetwork::onConnectSucceed);
        connect(socket.get(), &QTcpSocket::disconnected, this, &ChngNetwork::onConnectionClose);
        connect(socket.get(), &QTcpSocket::errorOccurred, this, &ChngNetwork::onConnectFail);
        socket->connectToHost(addr,port.toInt());
    }else if(role == tcp_role_server){
        this->startServer(addr,port);
    }
}

void ChngNetwork::send(QString& msg)
{
    socket->write(msg.toStdString().c_str());
    socket->flush();
}

void ChngNetwork::disconnect()
{
    if(role == tcp_role_client){
        socket->disconnectFromHost();
    }else{
        stopServer();
    }
}

const QString &ChngNetwork::getMessage() const
{
    return message;
}

void ChngNetwork::startServer(QString addr, QString port)
{
    server.reset(new QTcpServer(this));
    if( server->listen(QHostAddress(addr), (quint16)port.toInt()) ){
        //成功开启监听
        qDebug() << "successfully start listening";
        listening = true;
        connected = false;
        emit onServerListenStart();//通知UI
        connect(server.get(), &QTcpServer::newConnection, this, &ChngNetwork::onNewConnection);
    }else{
        //开启监听失败
        listening = false;
        connected = false;
        emit onConnectionError(server->errorString());//通知UI
    }
}

void ChngNetwork::stopServer()
{
    server->close();
    listening = false;
    connected = false;
    server.release();
    emit onServerStop();//通知UI
}

void ChngNetwork::onNewMessage()
{
    message = socket->readAll();
    emit onMessage();//通知UI
}

void ChngNetwork::onConnectSucceed()
{
    qDebug() << "client connection established";
    connected = true;
    emit onConnectionEstablished();//通知UI
}

void ChngNetwork::onConnectFail(QAbstractSocket::SocketError err)
{
    qDebug() << "connection failed, code:" << err;
    connected = false;
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    emit onConnectionError(metaEnum.valueToKey(err));//通知UI
}

void ChngNetwork::onConnectionClose()
{
    qDebug() << "connection closed";
    connected = false;
    if(role == tcp_role_server){
        stopServer();
    }else{
        socket.release();
    }
    emit onConnectionClosed();  //通知UI
}

void ChngNetwork::onNewConnection()
{
    qDebug() << "server got new connection";
    connected = true;
    this->socket.reset(server->nextPendingConnection());
    server->pauseAccepting();   //不再接受其他连接
    connect(socket.get(), &QTcpSocket::readyRead, this, &ChngNetwork::onNewMessage);
    connect(socket.get(), &QTcpSocket::disconnected, this, &ChngNetwork::onConnectionClose);
    emit onConnectionEstablished();//通知UI
    socket->write("Helle, you have successfully connected to the server.\n");
}
