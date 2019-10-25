#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>
class server;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onLogMsgReceived(const QString &text);
    void onUserEntered(const QString &username);

private slots:
    void on_exitButton_clicked();

private:
    Ui::MainWindow *ui;
    QScopedPointer<server> s;
};

#endif // MAINWINDOW_H
