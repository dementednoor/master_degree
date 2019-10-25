#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "server.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //s.reset(new server());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onLogMsgReceived(const QString &text)
{
    ui->logBrowser->append(text);
}

void MainWindow::onUserEntered(const QString &username)
{
    // ui->usersTable->cr
}

void MainWindow::on_exitButton_clicked()
{
    close();
}
