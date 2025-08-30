#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr);
    Q_INVOKABLE void startServer();

signals:
    void dataReceived(const QString &data);

private slots:
    void onNewConnection();
    void onReadyRead();

private:
    QTcpServer *server;
    QTcpSocket *clientSocket;
};

#endif // NETWORKMANAGER_H
