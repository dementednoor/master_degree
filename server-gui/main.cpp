#include "mainwindow.h"
#include <QApplication>
#include "server.h"
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    server s;
    QObject::connect(&s, SIGNAL(logAppend(QString)), &w, SLOT(onLogMsgReceived(QString)));
    // QObject::connect(&s, SIGNAL(logAppend(QString)), &w, SLOT(onUserEntered(QString)));
    w.show();
    s.startServer();

    return a.exec();
}
