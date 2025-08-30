#include "NetworkManager.h"
#include <QDebug>

NetworkManager::NetworkManager(QObject *parent) : QObject(parent), server(new QTcpServer(this)), clientSocket(nullptr) {
    connect(server, &QTcpServer::newConnection, this, &NetworkManager::onNewConnection);
}

void NetworkManager::startServer()
{
    if (!server->listen(QHostAddress::Any, 65432)) { // Lắng nghe trên tất cả các địa chỉ IP với cổng 65432
        qDebug() << "Server could not start! Error:" << server->errorString();
    } else {
        qDebug() << "Server started, listening on port 65432";
    }
}

void NetworkManager::onNewConnection()
{
    if (clientSocket && clientSocket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "A client is already connected. Rejecting new connection.";
        QTcpSocket *newClient = server->nextPendingConnection();
        newClient->disconnectFromHost();
        newClient->deleteLater();
        return;
    }

    clientSocket = server->nextPendingConnection();
    qDebug() << "New connection from:" << clientSocket->peerAddress().toString();
    connect(clientSocket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
}

void NetworkManager::onReadyRead()
{
    if (!clientSocket) {
        return;
    }

    QByteArray receivedData = clientSocket->readAll();
    QString dataString = QString::fromUtf8(receivedData);
    qDebug() << "Data received from client:" << dataString;

    emit dataReceived(dataString);
}
