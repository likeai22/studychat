#include "Headers/studychat.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    StudyChat chat;
    chat.show();

    return app.exec();
}
