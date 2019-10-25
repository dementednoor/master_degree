#ifndef ANALYSISPAGE_H
#define ANALYSISPAGE_H

#include <QWidget>
#include <QSqlDatabase>

namespace Ui {
class analysisPage;
}

class analysisPage : public QWidget
{
    Q_OBJECT

public:
    explicit analysisPage(QWidget *parent = 0);
    ~analysisPage();
    QString reportText;

signals:
    void awClose();
    void report();

private slots:
    void on_returnButton_clicked();

    void on_sendReportButton_clicked();

    void on_calcButton_clicked();

    QString resultComparison(float number);

    void reportLogUpdate(QString txt);

    void on_checkButton_clicked();

private:
    Ui::analysisPage *ui;
    QSqlDatabase connectedDB;
    int stratValue;
};

#endif // ANALYSISPAGE_H
