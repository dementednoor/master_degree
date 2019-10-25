#ifndef KEYENTERRECEIVER_H
#define KEYENTERRECEIVER_H

#include <QObject>

class keyEnterReceiver : public QObject
{
    Q_OBJECT
public:
    keyEnterReceiver();
    bool eventFilter(QObject* obj, QEvent* event);

signals:
    void entered();
};

#endif // KEYENTERRECEIVER_H
