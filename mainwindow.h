#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QTcpSocket>
#include <QFile>
#include "analysispage.h"
#include "keyenterreceiver.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    // QTcpSocket* socket;
    QByteArray serverReply;
    QSqlDatabase db;
    QByteArray reportData;

public slots:
        void sockReady();
        void sockDisc();
        void onSockConnected();


private slots:
    void on_enterButton_clicked();

    void on_exitButton_clicked();

    void errorBox(QString errorText);

    void on_analysisButton_clicked();

    void logFill(QString txt);

    void on_returnButton_clicked();

    void waitForConnection();

    void on_connectionButton_clicked();



    void on_returnButton_2_clicked();

    void on_sendButton_clicked();

    void on_logLoadButton_clicked();
    
    void historyUpdate(QString path, QString content);

    void on_analysisButton_2_clicked();

    void reportConvertion();

    void on_fastEnterButton_clicked();

    void on_pushButton_clicked();

    void on_analysisButton_3_clicked();

    void on_updateButton_clicked();

    void on_reportCheckButton_clicked();

private:
    Ui::MainWindow *ui;
    QString authLogin;
    QTcpSocket *socket;
    QScopedPointer<analysisPage> ap;
    keyEnterReceiver* retBut = new keyEnterReceiver;
    // bool eventFilter(QObject* obj, QEvent* event);
    // ("/home/noor/master/history.txt");
};

#endif // MAINWINDOW_H
