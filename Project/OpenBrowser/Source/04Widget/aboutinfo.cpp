// 01 Frame includes
#include "aboutinfo.h"
#include "ui_aboutinfo.h"
#include "DeeplvConfig.h"

AboutInfo::AboutInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AboutInfo) {
    ui->setupUi(this);
    this->Initial();
}

AboutInfo::~AboutInfo() {
    delete ui;
}

void AboutInfo::Initial() {
    ui->logo_label->setFixedSize(50, 50);
    this->setFixedSize(300, 75);
    ui->label_1->setText(ui->label_1->text().arg(QString::fromStdString(OpenBrowser_NAME)));
    ui->label_2->setText(ui->label_2->text().arg(QString::fromStdString(OpenBrowser_VER)));
}
