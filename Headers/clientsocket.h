#pragma once

#include <QTcpSocket>

class ClientSocket : public QTcpSocket {
Q_OBJECT
public:
    explicit ClientSocket(QObject *parent = nullptr);

signals:

    void newMessage(QString user, QString mess);

    void newUser(QString user);

    void deleteUser(QString user);

    void sendPrivateMessage(const QString &account, const QString &name, const QString &mess);

private slots:

    void readDataFromSocket();

    void correctDisconnect();

public slots:

    void sendMessageToAll(const QString& name, const QString& mess, const QStringList& users);

private:
    quint16 blockSize;
    QString userName;
};
