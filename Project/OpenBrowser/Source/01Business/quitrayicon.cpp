#pragma execution_character_set("utf-8")
// 01 Frame includes
#include "quitrayicon.h"

QScopedPointer<QUITrayIcon> QUITrayIcon::self_;
QUITrayIcon *QUITrayIcon::Instance() {
    if (self_.isNull()) {
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self_.isNull()) {
            self_.reset(new QUITrayIcon);
        }
    }
    return self_.data();
}

QUITrayIcon::QUITrayIcon(QObject *parent) : QObject(parent) {
    main_widget_ = nullptr;
    tray_icon_ = new QSystemTrayIcon(this);
    connect(tray_icon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(SlotIconIsActived(QSystemTrayIcon::ActivationReason)));
    menu_ = new QMenu(QApplication::desktop());
    exit_direct_ = true;
}

void QUITrayIcon::SlotIconIsActived(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick: {
                main_widget_->showNormal();
                break;
            }
        default:
            break;
    }
}

void QUITrayIcon::SetExitDirect(bool exitDirect) {
    if (this->exit_direct_ != exitDirect) {
        this->exit_direct_ = exitDirect;
    }
}

void QUITrayIcon::SetMainWidget(QWidget *mainWidget) {
    this->main_widget_ = mainWidget;
    menu_->addAction("主界面", mainWidget, SLOT(SlotMinClicked()));
    if (exit_direct_) {
        menu_->addAction("退出", this, SLOT(SlotCloseAll()));
    } else {
        menu_->addAction("退出", this, SIGNAL(TrayIconExit()));
    }
    tray_icon_->setContextMenu(menu_);
}

void QUITrayIcon::ShowMessage(
    const QString &title, const QString &msg,
    QSystemTrayIcon::MessageIcon icon, int msecs) {
    tray_icon_->showMessage(title, msg, icon, msecs);
}

void QUITrayIcon::SetIcon(const QString &strIcon) {
    tray_icon_->setIcon(QIcon(strIcon));
}

void QUITrayIcon::ToolTip(const QString &tip) {
    tray_icon_->setToolTip(tip);
}

void QUITrayIcon::SlotsetVisible(bool visible) {
    tray_icon_->setVisible(visible);
}

void QUITrayIcon::SlotCloseAll() {
    tray_icon_->hide();
    tray_icon_->deleteLater();
    exit(0);
}


