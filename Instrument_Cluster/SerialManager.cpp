#include "SerialManager.h"
#include <QDebug>

SerialManager::SerialManager(QObject *parent) : QObject(parent), serial(new QSerialPort(this)) {}

void SerialManager::start()
{
    serial->setPortName("COM6");  // Cổng của ESP32 trên Ubuntu
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (serial->open(QIODevice::ReadWrite)) {
        qDebug() << "[C++] Đã kết nối thành công! Port:" << serial->portName();
        qDebug() << "[C++] Cấu hình:"
                 << serial->baudRate() << serial->dataBits()
                 << serial->parity() << serial->stopBits();

        connect(serial, &QSerialPort::readyRead, this, &SerialManager::onReadyRead);
    } else {
        qDebug() << "[C++] Không thể mở cổng serial:" << serial->errorString();
    }
}

void SerialManager::sendData(const QString &data)
{
    if (serial->isOpen()) {
        QByteArray bytes = data.toUtf8();
        bytes.append('\n');  // Kết thúc dòng
        serial->write(bytes);
        qDebug() << "[C++] Sent:" << bytes;
    } else {
        qDebug() << "[C++] Cổng serial chưa mở.";
    }
}

void SerialManager::onReadyRead()
{
    buffer += QString::fromUtf8(serial->readAll());

    int index;
    while ((index = buffer.indexOf('\n')) != -1) {
        QString line = buffer.left(index).trimmed();
        buffer.remove(0, index + 1);

        if (!line.isEmpty()) {
            qDebug() << "[C++] Received:" << line;
            emit signalChanged(line);
        }
    }
}
