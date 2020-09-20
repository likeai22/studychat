#pragma once

#include <QTcpServer>
#include <map>
#include "clientsocket.h"


class Server : public QTcpServer {
Q_OBJECT
public:
    explicit Server(quint16 inputPort, QObject *parent = nullptr);

    ~Server() override;

signals:

    void sendMessageToAll(QString name, QString mess, QStringList users);

public slots:

private:
    void incomingConnection(int sockedID);

    QStringList GetUserList();

    std::map<ClientSocket *, QString> clientsMap;

private slots:

    void newUser(QString name);

    void newMessage(QString name, QString mess);

    void deleteUser(const QString &user);

    void sendPrivateMessage(const QString &account, const QString &name, const QString &mess);
};
