#ifndef REPORT_H
#define REPORT_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>

class Report
{
public:
    void report();
    QString senderName; // имя отправителя донесения
    unsigned short criteria_r; // вес критерия
    unsigned short targetValue; // оценка цели
    unsigned short crisisValue; // оценка кризиса, вводится вручную
    unsigned short strategyValue; // оценка стратегии
#define d targetValue
#define q strategyValue
#define v crisisValue


};

#endif // REPORT_H
