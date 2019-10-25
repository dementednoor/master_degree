#include "server.h"
#include <QTcpSocket>
#include <QRegExp>

server::server(QObject *parent) {}

server::~server() {}

void server::startServer()
{

    if (this->listen(QHostAddress::Any, 5555)) logFill("Listening...");

    else logFill("Not listening");
}

void server::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *clientSocket = new QTcpSocket(this);
    clientSocket->setSocketDescriptor(socketDescriptor);
    clients.insert(clientSocket);
    // MainWindow::ui->logBrowser->append("New client from:" + clientSocket->peerAddress().toString());
    logFill("New client from: " + clientSocket->peerAddress().toString());
    connect(clientSocket,SIGNAL(readyRead()), this, SLOT(socketReady()));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(socketDisc()));

    logFill("Connection status is sent");
}

void server::socketReady()
{
    /*  QByteArray username;
    username = socket->readAll();
    logFill(QString::fromLocal8Bit(username) + " has connected");
    QByteArray ba = (QString("[%1] %2").arg(QDateTime::currentDateTime().toString("hh:mm:ss")).arg("Соединение установлено")).toLocal8Bit();
    socket->write(ba.data());
    */
    QTcpSocket *client = (QTcpSocket*)sender(); // выделяем отдельный сокет-сендер
    while(client->canReadLine()) { // если в сокете что-то записано,
        QString line = QString::fromUtf8(client->readLine()).trimmed(); // записываем содержимое сокета в строку
        logFill("Read line:" + line); // выводим в консоль то, что записано

        QRegExp meRegex("^/me:(.*)$");

        if(meRegex.indexIn(line) != -1) // если в сокет записан юзернейм
        {
            QString user = meRegex.cap(1); // сохраняем его
            users[client] = user; // назначаем значение ключу в словаре
            foreach(QTcpSocket *client, clients)
                client->write(QString("Server:" + user + " has joined.\n").toUtf8()); // высылаем всем юзерам сообщение о том, что
            // такой-то пользователь присоединился к серверу
            sendUserList();

        }
        else if(users.contains(client)) // если этот педик со своим сокетом уже числится среди активных пользователей
        {
            // выводим в консоль его имя и месседж
            QString message = line;
            QString user = users[client];
            // qDebug() << "User:" << user;
            // qDebug() << "Message:" << message;

            foreach(QTcpSocket *otherClient, clients)
                otherClient->write(QString(user + ":" + message + "\n").toUtf8()); // и во все остальные сокеты высылаем мсг, дабы
            // они его прочитали и вывели у себя
        }
        else //  в противном случае пишем, что выводится какая-то шляпа
        {
            qWarning() << "Got bad message from client:" << client->peerAddress().toString() << line;
        }
    }

}

void server::socketSend(const QByteArray &array)
{

}

void server::socketDisc()
{
    QTcpSocket *client = (QTcpSocket*)sender(); // sender возвращает указатель на объект, который эмитировал сигнал, в данном случае на
    // сокет, у которого наебнулся коннекшн
    logFill("User disconnected: " + client->peerAddress().toString());
    clients.remove(client);
    QString user = users[client];
    users.remove(client);
    foreach(QTcpSocket *client, clients)
        client->write(QString("Server:" + user + " has left.\n").toUtf8());
    client->deleteLater();
    if (!client->isValid())
        delete client;
}

void server::logFill(QString text)
{
    emit logAppend(QString("[%1] %2").arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                   .arg(text));
}

void server::sendUserList()
{
    QStringList userList;
    foreach(QString user, users.values())
        userList << user;

    foreach(QTcpSocket *client, clients)
        client->write(QString("/users:" + userList.join(",") + "\n").toUtf8());
}


