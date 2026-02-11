// 01 Frame includes
#include "formmaskwidget.h"
#include "ui_formmaskwidget.h"

FormMaskWidget::FormMaskWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMaskWidget) {
    ui->setupUi(this);
    this->setObjectName("MaskWidget");
    this->setProperty("can_move", true);
    this->setWindowFlags(Qt::FramelessWindowHint |
                         Qt::Tool |
                         Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_ShowModal, true);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    QDesktopWidget *deskdop = QApplication::desktop();
    move((deskdop->width() - this->width()) / 2,
         (deskdop->height() - this->height()) / 2);
    ui->progress_sequential->setMaxValue(100);
    connect(ui->btn_signout, &QPushButton::clicked,
            this, &FormMaskWidget::Slotsignout);
}

FormMaskWidget::~FormMaskWidget() {
    delete ui;
}

void FormMaskWidget::SlotsStackedWidgetNum(const int &num) {
    ui->stackedWidget->setCurrentIndex(num);
}

void FormMaskWidget::SlotThisCloseIn() {
    if (ui->stackedWidget->currentIndex() == 1) {
        emit SingnalCancelOut();
    }
    this->close();
}

void FormMaskWidget::SlotThreadAICurrent(const QString tmp_str, const qint32 tmp_int) {
    if (ui->stackedWidget->currentIndex() == 0) {
    }  else if (ui->stackedWidget->currentIndex() == 1) {
        if (tmp_str != "") {
            ui->label_sequential->setText(tmp_str);
        }
        if (tmp_int > -1 && tmp_int > ui->progress_sequential->getValue()) {
            ui->progress_sequential->setValue(tmp_int);
        }
        if (tmp_int == 100) {
            emit SingnalEndOut();
            this->SlotThisCloseIn();
        }
    }
}

void FormMaskWidget::SetCancleHide() {
    ui->btn_signout->hide();
}

void FormMaskWidget::Slotsignout() {
    this->SlotThisCloseIn();
}
