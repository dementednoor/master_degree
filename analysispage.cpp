#include "analysispage.h"
#include "ui_analysispage.h"
#include "mainwindow.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include "qcustomplot.h"

analysisPage::analysisPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::analysisPage)
{
    ui->setupUi(this);
    // ui->r_commonWeightLine->setEnabled(false);
    // ui->h_criteriaWeightLine->setEnabled(false);
    connectedDB = QSqlDatabase::addDatabase("QSQLITE");
    connectedDB.setDatabaseName("/home/noor/master_degree/master.db");
    connectedDB.open();
#define db connectedDB
}

analysisPage::~analysisPage()
{
    delete ui;
}

void analysisPage::on_returnButton_clicked()
{
    // emit awClose();
    qDebug() << "Окно анализа ситуации закрыто";
    this->close();

}

void analysisPage::on_sendReportButton_clicked()
{
    emit report();
    reportText = QStringLiteral("Ситуация: %1\nОценка: %2\n").arg(ui->strategyLine->text())
            .arg(stratValue);
}

void analysisPage::on_calcButton_clicked()
{
    /*
    r = sum (rk*n)
    h = r / sum (r)
    d = sum (x*h)
    q = d / v
     */
    ui->reportBrowser->clear();
    // ui->x_criteriaValueLine->setEnabled(false);
    QSqlQuery valuesCountRequest;
    valuesCountRequest.prepare("SELECT FROM CRITERIAS WHERE");
    // критерий - его важность (вес, в процентах) и его значение

    float rk = ui->rk_criteriaWeightLine->text().toFloat(); // вес критерия, применяемого к цели
    // int n = 1;
    int n = ui->n_marksNumber->text().toInt(); // количество юзеров, которые поставили эту оценку
    float r = rk*n; // расчет общего веса критериев
    // ui->r_commonWeightLine->setText(QString::number(r));
    float h = r; // здесь позже будет прописано суммирование всех критериев, а так это сумма весов
    // ui->h_criteriaWeightLine->setText(QString::number(h));

    float x = ui->x_criteriaValueLine->text().toFloat(); // значение критерия
    float d = x*h;
    ui->q_strategyValueLine->setText(QString::number(d));
    // ui->reportBrowser->append();
    reportLogUpdate(QStringLiteral("%1 - %2").arg("Стратегия ").arg(ui->strategyLine->text()));
    reportLogUpdate(QStringLiteral("%1 = %2").arg("Кризис ").arg("численное преимущество"));
    reportLogUpdate(QStringLiteral("%1 = %2").arg("Вес критерия").arg(rk));
    reportLogUpdate(QStringLiteral("%1 = %2").arg("Общий вес").arg(r));
    reportLogUpdate(QStringLiteral("%1 = %2").arg("Общее значение критериев").arg(h));
    reportLogUpdate(QStringLiteral("%1 = %2").arg("Оценка стратегии").arg(d));
    reportText = resultComparison(d);
    stratValue = d;
    // нам нужны только x, r, n
    QSqlQuery addToBase;
    addToBase.prepare("INSERT INTO Criterias (Name, Mark, Number, Value, Strategy)"
                      "VALUES (?, ?, ?, ?, ?)" );
    addToBase.addBindValue(ui->targetNameValue->text());
    addToBase.addBindValue(ui->rk_criteriaWeightLine->text().toFloat());
    addToBase.addBindValue(ui->n_marksNumber->text().toInt());
    addToBase.addBindValue(ui->x_criteriaValueLine->text().toFloat());
    addToBase.addBindValue(ui->strategyLine->text());
    addToBase.exec();
}

QString analysisPage::resultComparison(float number)
{
    if ((number > 80) && (number <= 100)) return "Ситуация критична \n";
    if ((number > 60) && (number < 80)) return "Ситуация близка к критичной\n";
    if ((number >40) && (number < 60)) return "Ситуация нестабильна на нескольких точках\n";
    if ((number >20) && (number <40)) return "Ситуация настабильна на одной точке\n";
    if (number< 20) return "Ситуация стабильна\n";
}

void analysisPage::reportLogUpdate(QString txt)
{
    ui->reportBrowser->append(txt);
}

/*
 * q - оценка стратегии разрешения кризиса
 * q = d / v
 *      d - оценка цели, которая реализуется внутри стратегии
 *          d = x*rk
 *              x - балльное / лексическое значение критерия
 *              rk = общий вес критерия
 *                  rk = r*n, r - вес критерия, n - количество ползователей, поставивших оценку
 *
 *
 * */

void analysisPage::on_checkButton_clicked()
{
    QSqlDatabase base;
    base = QSqlDatabase::addDatabase("QSQLITE");
    base.setDatabaseName("/home/noor/master_degree/master.db");
    base.open();
    qDebug() << "бд открыта - " << base.isValid();

    QString targetName = ui->targetNameValue->text();
    qDebug() << "проверочный";
    qDebug() << targetName;
    /*QSqlQuery q;
    q.exec("SELECT Mark FROM Criterias WHERE Name = 'П'");
    while (q.next()) {
        QString val = q.value(0).toString();
         qDebug() << "value - " + val;
    }
    // q.exec();*/

    QSqlQuery checkRequest;
    checkRequest.prepare("SELECT Mark, Number, Value FROM Criterias WHERE Name=?");
    checkRequest.addBindValue(targetName);
    checkRequest.exec();
    qDebug() << "query size = " + QString::number(checkRequest.size());

    while (checkRequest.next()) {
        // QString val = checkRequest.value(0).toString();
        // qDebug() << val;

         // qDebug() << "query size = " + checkRequest.size();
        if (checkRequest.size() == 0) QMessageBox::information(this, "So", "no matches", QMessageBox::Ok);

        else {
            QString mark = checkRequest.value(0).toString();
            qDebug() << "value - " + mark;
            ui->n_marksNumber->setText(checkRequest.value(1).toString());
            ui->d_targetValueLine->setText(checkRequest.value(2).toString());
        }
    }
}
