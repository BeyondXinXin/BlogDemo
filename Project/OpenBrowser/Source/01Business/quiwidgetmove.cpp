// 01 Frame includes
#include "quiwidgetmove.h"

QUIWidgetMove *QUIWidgetMove::self_ = nullptr;
QUIWidgetMove *QUIWidgetMove::Instance() {
    if (!self_) {
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self_) {
            self_ = new QUIWidgetMove;
        }
    }
    return self_;
}

QUIWidgetMove::QUIWidgetMove(QObject *parent) : QObject(parent) {
}

bool QUIWidgetMove::eventFilter(QObject *obj, QEvent *evt) {
    QWidget *w = static_cast<QWidget *>(obj);

    if (!w->property("can_move").toBool()) {
        return QObject::eventFilter(obj, evt);
    }
    static QPoint mousePoint;
    static bool mousePressed = false;
    QMouseEvent *event = static_cast<QMouseEvent *>(evt);
    if (event->type() == QEvent::MouseButtonPress) {
        if (event->button() == Qt::LeftButton) {
            mousePressed = true;
            mousePoint = event->globalPos() - w->pos();
            return true;
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        mousePressed = false;
        return true;
    } else if (event->type() == QEvent::MouseMove) {
        if (mousePressed && (event->buttons() & Qt::LeftButton)) {
            w->move(event->globalPos() - mousePoint);
            return true;
        }
    }
    return QObject::eventFilter(obj, evt);
}

void QUIWidgetMove::Run() {
    qApp->installEventFilter(this);
}

