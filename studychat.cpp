#include "Headers/studychat.h"
#include "ui_studychat.h"
#include <QMessageBox>

StudyChat::StudyChat(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::StudyChat),
        isPrivateMessage(false) {
    ui->setupUi(this);
    clientSocket = new QTcpSocket(this);
    blockSize = 0;

    connect(clientSocket, SIGNAL(connected()), this, SLOT(changeStatus()));
    connect(clientSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(readDataFromSocket()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(sendMessage()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(connectToServer()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(iAmServer()));

    connect(ui->radioButtonPrivateMessage, SIGNAL(clicked()), this, SLOT(onPrivateRBClicked()));
    connect(ui->radioButtonCommonMessage, SIGNAL(clicked()), this, SLOT(onCommonRBClicked()));
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem * )), this,
            SLOT(onItemClicked(QListWidgetItem * )));

}

StudyChat::~StudyChat() {
    delete ui;
}

void StudyChat::changeStatus() {
    ui->label_3->setText("Online");

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    quint16 size = sizeof(QChar('N')) + sizeof(ui->lineEdit_3->text());
    out << size << QChar('N') << ui->lineEdit_3->text();

    clientSocket->write(data);
}

void StudyChat::readDataFromSocket() {
    QDataStream in(clientSocket);

    if (blockSize == 0) {
        if (clientSocket->bytesAvailable() < sizeof(quint16))
            return;

        in >> blockSize;
    }

    if (clientSocket->bytesAvailable() < blockSize)
        return;

    QChar ch;
    QString account;
    QString name;
    QString message;
    QStringList listOfAcc;

    in >> ch;

    if (ch == 'S') {
        in >> name >> message >> listOfAcc;
    } else if (ch == 'P') {
        in >> account >> name >> message >> listOfAcc;
    } else {
        QMessageBox::critical(this, "Error", "Unknown message from the server");
        closeConnection();
        return;
    }

    if (!listOfAcc.empty()) {
        ui->listWidget->clear();
        ui->listWidget->addItems(listOfAcc);
    }


    if (!name.isEmpty() && !message.isEmpty()) {
        ui->textEdit->append(QString("%1 : %2").arg(name).arg(message));
    }

    blockSize = 0;
}

void StudyChat::closeConnection() {

    clientSocket->close();
    ui->label_3->setText("Offline");
}

void StudyChat::updateOpponent(const QString &name) {
    accountName = name;
    ui->labelUserName->setText(tr("Account: ") + accountName);
}

void StudyChat::error() {
    QMessageBox::critical(this, "Error", "Connection failed");
    ui->pushButton_3->setEnabled(true);
}


void StudyChat::connectToServer() {
    if (clientSocket->isOpen()) {
        QMessageBox::information(this, "Information", "You're already connected");
        return;
    }

    if (ui->lineEdit->text().isEmpty() || ui->lineEdit_2->text().isEmpty()) {
        QMessageBox::information(this, "Information", "Enter IP and port");
        return;
    }

    if (ui->lineEdit_3->text().isEmpty()) {
        QMessageBox::information(this, "Information", "Enter your name");
        return;
    }

    ui->pushButton_3->setEnabled(false);

    clientSocket->connectToHost(ui->lineEdit->text(), ui->lineEdit_2->text().toInt());
}

void StudyChat::iAmServer() {
    ui->frame->setEnabled(false);
    server = new Server(4545, this);
}

void StudyChat::onItemClicked(QListWidgetItem *item) {
    updateOpponent(item->data(Qt::DisplayRole).toString());
}

void StudyChat::onPrivateRBClicked() {
    isPrivateMessage = true;
}

void StudyChat::onCommonRBClicked() {
    accountName.clear();
    isPrivateMessage = false;
    updateOpponent(tr("<not selected>"));
}

void StudyChat::sendMessage() {
    if (!clientSocket->isOpen()) {
        QMessageBox::information(this, "Information", "Can't connect to the server");
        return;
    }

    if (ui->textEdit_2->toPlainText().isEmpty())
        return;

    if (ui->lineEdit_3->text().isEmpty()) {
        QMessageBox::information(this, "Information", "Enter your name");
        return;
    }

    QChar sym = isPrivateMessage ? QChar('P') : QChar('S');

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    quint16 size = sizeof(sym) + sizeof(accountName) +
                   sizeof(ui->lineEdit_3->text()) +
                   sizeof(ui->textEdit_2->toPlainText());

    out << size << sym << accountName << ui->lineEdit_3->text() << ui->textEdit_2->toPlainText();

    clientSocket->write(data);

    ui->textEdit_2->clear();
}
