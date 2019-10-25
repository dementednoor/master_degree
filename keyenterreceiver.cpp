#include "keyenterreceiver.h"
#include "mainwindow.h"
#include <QEvent>
#include <QKeyEvent>
#include <QMessageBox>

keyEnterReceiver::keyEnterReceiver()
{

}

bool keyEnterReceiver::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        if ((key->key() == Qt::Key_Enter) || (key->key() == Qt::Key_Return)) {
            // QMessageBox::information(this, "kek", "entered", QMessageBox::Ok);
            emit entered();
        }
        else {
            return QObject::eventFilter(obj, event);
        }
        return true;
    }
    else {
        return QObject::eventFilter(obj, event);
    }
    return false;
}
