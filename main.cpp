#include "mainwindow.h"
#include <QApplication>
#include <QObject>
#include "analysispage.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    // analysisPage ap;
    w.show();

    return a.exec();
}
