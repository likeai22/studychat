#include "Headers/server.h"
#include <QDataStream>
#include <utility>

Server::Server(quint16 inputPort, QObject *parent) :
        QTcpServer(parent) {
    listen(QHostAddress::Any, inputPort);
}

Server::~Server() = default;

void Server::incomingConnection(int sockedID) {
    auto *newClient = new ClientSocket(this);
    newClient->setSocketDescriptor(sockedID);
    connect(newClient, SIGNAL(newUser(QString)), this, SLOT(newUser(QString)));
    connect(newClient, SIGNAL(newMessage(QString, QString)), this, SLOT(newMessage(QString, QString)));
    connect(newClient, SIGNAL(sendPrivateMessage(QString, QString, QString)), this,
            SLOT(sendPrivateMessage(QString, QString, QString)));
    connect(this, SIGNAL(sendMessageToAll(QString, QString, QStringList)), newClient,
            SLOT(sendMessageToAll(QString, QString, QStringList)));
    connect(newClient, SIGNAL(deleteUser(QString)), this, SLOT(deleteUser(QString)));
}

QStringList Server::GetUserList() {
    QStringList users;
    for (auto & it : clientsMap) {
        users << it.second;
    }

    return users;
}

void Server::newUser(QString name) {
    clientsMap[(ClientSocket *) sender()] = std::move(name);

    emit sendMessageToAll("", "", GetUserList());
}

void Server::deleteUser(const QString& user) {
    for (auto it = clientsMap.begin(); it != clientsMap.end(); ++it) {
        if (it->first == sender() && it->second == user) {
            clientsMap.erase(it);
            break;
        }
    }
}

void Server::newMessage(QString name, QString mess) {
    emit sendMessageToAll(std::move(name), std::move(mess), GetUserList());
}

void Server::sendPrivateMessage(const QString &account, const QString &name, const QString &mess) {
    for (auto & it : clientsMap) {
        if (it.second == account) {
            QByteArray data;
            QDataStream out(&data, QIODevice::WriteOnly);

            quint16 size = sizeof(QChar('P')) + sizeof(account) + sizeof(name) + sizeof(mess);

            out << size << QChar('P') << account << name << mess;
            it.first->write(data);

            break;
        }
    }
}
