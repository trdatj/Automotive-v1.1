#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

class SerialManager : public QObject
{
    Q_OBJECT
public:
    explicit SerialManager(QObject *parent = nullptr);
    Q_INVOKABLE void start();

    Q_INVOKABLE void sendData(const QString &data);

signals:
    void signalChanged(const QString &data);  // Emit khi có dữ liệu mới

private slots:
    void onReadyRead();

private:
    QSerialPort *serial;
    QString buffer;
};

#endif // SERIALMANAGER_H
