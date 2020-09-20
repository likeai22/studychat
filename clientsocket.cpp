#include "Headers/clientsocket.h"
#include <QDataStream>

ClientSocket::ClientSocket(QObject *parent) :
        QTcpSocket(parent) {
    blockSize = 0;

    connect(this, SIGNAL(disconnected()), this, SLOT(correctDisconnect()));
    connect(this, SIGNAL(readyRead()), this, SLOT(readDataFromSocket()));
}

void ClientSocket::correctDisconnect() {
    emit deleteUser(userName);
    deleteLater();
}

void ClientSocket::readDataFromSocket() {
    QDataStream inSocket(this);
    if (blockSize == 0) {
        if (bytesAvailable() < sizeof(quint16))
            return;

        inSocket >> blockSize;
    }

    if (bytesAvailable() < blockSize)
        return;

    QByteArray data = readAll();
    QDataStream in(&data, QIODevice::ReadOnly);

    QChar ch;
    QString account;
    QString name;
    QString message;

    in >> ch;

    if (ch == 'S') {
        in >> account >> name >> message;
        emit newMessage(name, message);
    } else if (ch == 'P') {
        in >> account >> name >> message;
        emit sendPrivateMessage(account, name, message);
    } else if (ch == 'N') {
        in >> name;
        userName = name;
        emit newUser(name);
    } else {
        return;
    }

    blockSize = 0;
}

void ClientSocket::sendMessageToAll(const QString& name, const QString& mess, const QStringList& users) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);

    quint16 size = sizeof(QChar('S')) + sizeof(name) + sizeof(mess) + sizeof(users);

    out << size << QChar('S') << name << mess << users;

    write(data);
}
