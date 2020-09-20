#pragma once

#include <QWidget>
#include <QTcpSocket>
#include <QListWidgetItem>
#include "server.h"

namespace Ui {
    class StudyChat;
}

class StudyChat : public QWidget {
Q_OBJECT

public:
    explicit StudyChat(QWidget *parent = nullptr);

    ~StudyChat() override;

private:
    Ui::StudyChat *ui;
    QTcpSocket *clientSocket;
    quint16 blockSize;
    Server *server{};

    QString accountName;
    bool isPrivateMessage;

    void closeConnection();

    void updateOpponent(const QString &name);

private slots:

    void changeStatus();

    void readDataFromSocket();

    void error();

    void sendMessage();

    void connectToServer();

    void iAmServer();

    void onItemClicked(QListWidgetItem *item);

    void onPrivateRBClicked();

    void onCommonRBClicked();

};
