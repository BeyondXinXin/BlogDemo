// 01 Frame includes
#include "formleft.h"
#include "ui_formleft.h"

FormLeft::FormLeft(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLeft) {
    ui->setupUi(this);
    initFrom();
    InitMdoe();
    InitImgae();
    InitDcm();
    InitImgaes();
}

FormLeft::~FormLeft() {
    delete ui;
}

void FormLeft::initFrom() {
    // CPU内存显示控件
    ui->label_cpu->Start(1000);
    // 滑动导航条控件
    ui->slider_bar->setItems("Image      Brower;"
                             "Model        Brower;"
                             "Dcm          Brower;"
                             "Images    Brower");
    connect(ui->slider_bar, &SliderBar::currentItemChanged,
            this, &FormLeft::SingalSliderBarMovtToOut);
    connect(this, &FormLeft::SingalSliderBarMovtToIn,
            ui->slider_bar, &SliderBar::setCurrentIndex);
    // 页面切换按钮逻辑
    QPushButton *tmp = new QPushButton;
    tmp->setCheckable(1);
    ui->buttongroup->addButton(tmp, 5);
    LeftFormShow(0);
    connect(ui->btn_dcm, &QPushButton::clicked,
            this, &FormLeft::SlotLeftShowChangeIn);
    connect(ui->btn_image, &QPushButton::clicked,
            this, &FormLeft::SlotLeftShowChangeIn);
    connect(ui->btn_images, &QPushButton::clicked,
            this, &FormLeft::SlotLeftShowChangeIn);
    connect(ui->btn_mode, &QPushButton::clicked,
            this, &FormLeft::SlotLeftShowChangeIn);
}

void FormLeft::InitMdoe() {
    // Mode 逻辑
    QList<QPushButton *> btns = ui->widget_mode->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        qint32 tmp = btn->objectName().remove(0, 8).toInt();
        connect(btn, &QPushButton::clicked,
                this, &FormLeft::SlotModeBrowserAllBtnEnabledFalse);
        connect(btn, &QPushButton::clicked, this, [ = ] {
            emit SignalsModeBrowserOut(tmp);
            emit SingalSliderBarMovtToOut(1);
            emit SingalSliderBarMovtToIn(1);
        });
    }
}


void FormLeft::InitImgae() {
    // Image 逻辑
    image_handle = new FormImageHandle;
    connect(image_handle, &FormImageHandle::SingalImageMenuOut,
            this, &FormLeft::SingalImageBrowserOut);
    image_handle->hide();

    QList<QPushButton *> imgbtns = ui->widget_image->findChildren<QPushButton *>();
    foreach (QPushButton *btn, imgbtns) {

        if (btn->objectName().contains("btn_image")) {
            qint32 tmp = btn->objectName().remove(0, 9).toInt();
            connect(btn, &QPushButton::clicked, this, [ = ] { // v自动提取连通域
                emit SingalImageBrowserOut(tmp);
            });
        }

        if (btn->objectName().contains("btn_menuimage")) {
            qint32 tmp = btn->objectName().remove(0, 13).toInt();
            connect(btn, &QPushButton::clicked, this, [ = ] { // v自动提取连通域
                image_handle->ShowImageHandel(tmp);
            });
        }

    }
}

void FormLeft::InitDcm() {

}

void FormLeft::InitImgaes() {
    // images 逻辑
    QList<QPushButton *> btns = ui->widget_images->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        qint32 tmp = btn->objectName().remove(0, 10).toInt();
        connect(btn, &QPushButton::clicked, this, [ = ] {
            emit SingalSliderBarMovtToOut(3);
            emit SignalImagesBrowserOut(tmp);
        });
    }
}

// 显示切换
void FormLeft::SlotLeftShowChangeIn() {
    if (ui->btn_image == QObject::sender()) {
        ui->widget_image->isHidden() ? LeftFormShow(1) : LeftFormShow(0);
    } else if (ui->btn_mode == QObject::sender()) {
        ui->widget_mode->isHidden() ? LeftFormShow(2) : LeftFormShow(0);
    } else if (ui->btn_dcm == QObject::sender()) {
        ui->widget_dcm->isHidden() ? LeftFormShow(3) : LeftFormShow(0);
    } else if (ui->btn_images == QObject::sender()) {
        ui->widget_images->isHidden() ? LeftFormShow(4) : LeftFormShow(0);
    }
}

void FormLeft::LeftFormShow(const int browser) {
    ui->widget_dcm->hide();
    ui->widget_image->hide();
    ui->widget_images->hide();
    ui->widget_mode->hide();

    switch (browser) {
        case 1: {// Image 前处理
                ui->slider_bar->moveTo(0);
                ui->widget_image->show();
                break;
            }
        case 2: {// Mode 前处理
                ui->slider_bar->moveTo(1);
                ui->widget_mode->show();
                break;
            }
        case 3: {// Dcm 前处理
                ui->slider_bar->moveTo(2);
                ui->widget_dcm->show();

                break;
            }
        case 4: {// Images 前处理
                ui->slider_bar->moveTo(3);
                ui->widget_images->show();
                break;
            }
        default: {
                ui->label_dcm->hide();
                ui->label_image->hide();
                ui->label_images->hide();
                ui->label_mode->hide();
                ui->label_cpu->hide();
                ui->textedit_information->hide();
                ui->slider_bar->hide();
                ui->buttongroup->button(5)->setChecked(1);
                return;
            }
    }
    ui->label_dcm->show();
    ui->label_image->show();
    ui->label_images->show();
    ui->textedit_information->show();
    ui->label_mode->show();
    ui->slider_bar->show();
    ui->label_cpu->show();
}

// 正常提示信息(白色)
void FormLeft::SlotPromptInformation(const QString &text) {
    QString output = QString("<font color=\"white\">%1</font><br>")
                     .arg(text);
    output = output.replace("\n", "<br/>");
    ui->textedit_information->setText(output);
}

// mode 按钮无法按下
void FormLeft::SlotModeBrowserAllBtnEnabledFalse() {
    QList<QPushButton *> btns = ui->widget_mode->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        btn->setEnabled(false);
    }
}

// mode 按钮正常按下
void FormLeft::SlotModeBrowserBtnEnabledTrue() {
    QList<QPushButton *> btns = ui->widget_mode->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        btn->setEnabled(true);
    }
}


