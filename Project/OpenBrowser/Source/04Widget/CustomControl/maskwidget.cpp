// 01 Frame includes
#include "maskwidget.h"

MaskWidget *MaskWidget::self_ = nullptr;
MaskWidget::MaskWidget(QWidget *parent) : QWidget(parent) {
    main_widget_ = nullptr;
    SlotSetOpacity(0.7);
    SlotSetBgColor(QColor(0, 0, 0));
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    qApp->installEventFilter(this);
}

void MaskWidget::SlotSetMainWidget(QWidget *mainWidget) {
    if (this->main_widget_ != mainWidget) {
        this->main_widget_ = mainWidget;
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    }
}

void MaskWidget::SlotSetDialogNames(const QStringList &dialogNames) {
    if (this->dialog_names_ != dialogNames) {
        this->dialog_names_ = dialogNames;
    }
}

void MaskWidget::SlotSetOpacity(double opacity) {
    this->setWindowOpacity(opacity);
}

void MaskWidget::SlotSetBgColor(const QColor &bgColor) {
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Background, bgColor);
    this->setPalette(palette);
}

void MaskWidget::showEvent(QShowEvent *) {
    if (main_widget_ != nullptr) {
        if (main_widget_->isFullScreen()) {
            this->showFullScreen();
        } else {
            this->setGeometry(main_widget_->geometry());
        }
    }
}

bool MaskWidget::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::Show) {
        if (dialog_names_.contains(obj->objectName())) {
            this->show();
        }
    } else if (event->type() == QEvent::Hide) {
        if (dialog_names_.contains(obj->objectName())) {
            this->hide();
        }
    }
    return QObject::eventFilter(obj, event);
}
