#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QWidget>
#include <QScopedPointer>
#include <QDateTime>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMap>
#include <QSet>

class server : public QTcpServer
{
    Q_OBJECT
public:
    server(QObject *parent =0);
    ~server();

    // QTcpSocket* clientSocket;
    QByteArray data;

signals:
    void logAppend(const QString &text);

public slots:
    void startServer();
    void incomingConnection(qintptr socketDescriptor);
    void socketReady(); // слот на readyRead
    void socketSend(const QByteArray& array);
    void socketDisc(); // слот на disconnected
    void logFill(QString text);
    void sendUserList();

private:
    //QScopedPointer<QTcpSocket> _socket;
    QSet<QTcpSocket*> clients;
    QMap<QTcpSocket*, QString> users;
};

#endif // SERVER_H
