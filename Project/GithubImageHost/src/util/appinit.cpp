#include "appinit.h"
#include "qapplication.h"
#include "qevent.h"
#include "qmutex.h"
#include "qwidget.h"
#include <QApplication>
#include <QDebug>
#include <QPalette>
#include <QWidget>

QScopedPointer<QUIHelper> QUIHelper::self;
QUIHelper *QUIHelper::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new QUIHelper);
        }
    }

    return self.data();
}

QUIHelper::QUIHelper(QObject *parent)
  : QObject(parent)
{
}

bool QUIHelper::eventFilter(QObject *watched, QEvent *event)
{
    QWidget *w = (QWidget *)(watched);
    if (!w->property("canMove").toBool()) {
        return QObject::eventFilter(watched, event);
    }

    static QPoint mousePoint;
    static bool mousePressed = false;

    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    if (mouseEvent->type() == QEvent::MouseButtonPress) {
        if (mouseEvent->button() == Qt::LeftButton) {
            mousePressed = true;
            mousePoint = mouseEvent->globalPos() - w->pos();
            return true;
        }
    } else if (mouseEvent->type() == QEvent::MouseButtonRelease) {
        mousePressed = false;
        return true;
    } else if (mouseEvent->type() == QEvent::MouseMove) {
        if (mousePressed && (mouseEvent->buttons() == Qt::LeftButton)) {
            w->move(mouseEvent->globalPos() - mousePoint);
            return true;
        }
    }

    return QObject::eventFilter(watched, event);
}

void QUIHelper::start()
{
    qApp->installEventFilter(this);
}

void QUIHelper::SetStyle(QWidget *wid, const QString &qssFile)
{
    QFile file(qssFile);
    if (file.open(QFile::ReadOnly)) {
        QString qss;
        QStringList list;
        QTextStream in(&file);
        //        in.setCodec("utf-8");
        //        while (!in.atEnd()) {
        //            QString line;
        //            in >> line;
        //            list << line;
        //        }
        //        qss = list.join("\n");
        qss = in.readAll();
        QString paletteColor = qss.mid(15, 7);
        if (wid) {
            wid->setPalette(QPalette(QColor(paletteColor)));
            wid->setStyleSheet(qss);
            wid->setAutoFillBackground(true);
        } else {
            qApp->setPalette(QPalette(QColor(paletteColor)));
            qApp->setStyleSheet(qss);
        }

        file.close();
    }
}
