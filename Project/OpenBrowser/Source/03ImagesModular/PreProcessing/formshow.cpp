#pragma execution_character_set("utf-8")
// 01 Frame includes
#include "formshow.h"
#include "ui_formshow.h"
#include "quihelper.h"


FormShow::FormShow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormShow) {
    ui->setupUi(this);
    this->Initial();
}
FormShow::~FormShow() {
    delete ui;
}

void FormShow::Initialization() {
    connect(ui->frmshowpushButton_8, &QPushButton::clicked,
            this, &FormShow::SlotPushButtonClicked);
    connect(ui->frmshowpushButton_9, &QPushButton::clicked,
            this, &FormShow::SlotPushButtonClicked);
    connect(ui->frmshowpushButton_10, &QPushButton::clicked,
            this, &FormShow::SlotPushButtonClicked);
    connect(ui->frmshowpushButton_11, &QPushButton::clicked,
            this, &FormShow::SlotPushButtonClicked);
    connect(ui->frmshowpushButton_12, &QPushButton::clicked,
            this, &FormShow::SlotPushButtonClicked);
    for (int i = 0; i < video_count_; i++) {
        QLabel *lab = new QLabel;
        lab->setProperty("formshow", "lab");
        lab->setObjectName(QString("video%1").arg(i + 1));
        lab->installEventFilter(this);
        lab->setFocusPolicy(Qt::StrongFocus);
        lab->setAlignment(Qt::AlignCenter);
        lab->setText(QString("通道 %1").arg(i + 1));
        lab->setScaledContents(true);
        widgets_.append(lab);
    }
    video_menu_ = new QMenu(this);
    QMenu *menu4 = video_menu_->addMenu("切换到4画面");
    menu4->addAction("通道1-通道4", this, SLOT(ShowVideo4()));
    menu4->addAction("通道5-通道8", this, SLOT(ShowVideo4()));
    menu4->addAction("通道9-通道12", this, SLOT(ShowVideo4()));
    menu4->addAction("通道13-通道16", this, SLOT(ShowVideo4()));
    QMenu *menu6 = video_menu_->addMenu("切换到6画面");
    menu6->addAction("通道1-通道6", this, SLOT(ShowVideo6()));
    menu6->addAction("通道6-通道11", this, SLOT(ShowVideo6()));
    menu6->addAction("通道11-通道16", this, SLOT(ShowVideo6()));
    QMenu *menu8 = video_menu_->addMenu("切换到8画面");
    menu8->addAction("通道1-通道8", this, SLOT(ShowVideo8()));
    menu8->addAction("通道9-通道16", this, SLOT(ShowVideo8()));
    QMenu *menu9 = video_menu_->addMenu("切换到9画面");
    menu9->addAction("通道1-通道9", this, SLOT(ShowVideo9()));
    menu9->addAction("通道8-通道16", this, SLOT(ShowVideo9()));
    video_menu_->addAction("切换到16画面", this, SLOT(ShowVideo16()));
    this->ShowVideoAll();
}

void FormShow::ShowImages(const QStringList filenames) {
    for (int i = 0; (i < filenames.size()) && (i < 16); ++i) {
        QString path = filenames.at(i);
        if (!path.isEmpty()) {
            QImage *img = new QImage(path);
            if ((img->isNull())) {
                QUIHelper::ShowMessageBoxError("打开图片错误！");
                delete img;
                return;
            }
            QLabel *lab = widgets_.at(i);
            lab->setPixmap(QPixmap::fromImage(*img));
            lab->setScaledContents(true);
        }
    }
}
bool FormShow::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::MouseButtonDblClick) {
        QLabel *widget = static_cast<QLabel *>(watched);
        if (!video_max_) {
            video_max_ = true;
            HideVideoAll();
            ui->formshow_gridLayout->addWidget(widget, 0, 0);
            widget->setVisible(true);
        } else {
            video_max_ = false;
            ShowVideoAll();
        }
    } else if (event->type() == QEvent::MouseButtonPress) {
        if (qApp->mouseButtons() == Qt::RightButton) {
            video_menu_->exec(QCursor::pos());
        }
    }
    return QWidget::eventFilter(watched, event);
}
void FormShow::Initial() {
    video_max_ = false;
    video_count_ = 16;
    video_type_ = "1_16";
}

void FormShow::SlotPushButtonClicked() {
    video_max_ = false;
    QString videoType;
    int index = 0;
    index = 0;
    if (QObject::sender() == ui->frmshowpushButton_8) {
        videoType = "1_4";
        if (this->video_type_ != videoType) {
            this->video_type_ = videoType;
            ChangeVideo4(index);
        }
    } else if(QObject::sender() == ui->frmshowpushButton_9) {
        videoType = "1_6";
        if (this->video_type_ != videoType) {
            this->video_type_ = videoType;
            ChangeVideo6(index);
        }
    } else if(QObject::sender() == ui->frmshowpushButton_10) {
        videoType = "1_8";
        if (this->video_type_ != videoType) {
            this->video_type_ = videoType;
            ChangeVideo8(index);
        }
    } else if(QObject::sender() == ui->frmshowpushButton_11) {
        videoType = "1_9";
        if (this->video_type_ != videoType) {
            this->video_type_ = videoType;
            ChangeVideo9(index);
        }
    } else if(QObject::sender() == ui->frmshowpushButton_12) {
        videoType = "1_16";
        if (this->video_type_ != videoType) {
            this->video_type_ = videoType;
            ChangeVideo16(index);
        }
    }
}


void FormShow::ShowVideoAll() {
    if (video_type_ == "1_4") {
        ChangeVideo4(0);
    } else if (video_type_ == "5_8") {
        ChangeVideo4(4);
    } else if (video_type_ == "9_12") {
        ChangeVideo4(8);
    } else if (video_type_ == "13_16") {
        ChangeVideo4(12);
    } else if (video_type_ == "1_6") {
        ChangeVideo6(0);
    } else if (video_type_ == "6_11") {
        ChangeVideo6(5);
    } else if (video_type_ == "11_16") {
        ChangeVideo6(10);
    } else if (video_type_ == "1_8") {
        ChangeVideo8(0);
    } else if (video_type_ == "9_16") {
        ChangeVideo8(8);
    } else if (video_type_ == "1_9") {
        ChangeVideo9(0);
    } else if (video_type_ == "8_16") {
        ChangeVideo9(7);
    } else if (video_type_ == "1_16") {
        ChangeVideo16(0);
    }
}
void FormShow::ShowVideo4() {
    video_max_ = false;
    QString videoType;
    int index = 0;
    QAction *action = static_cast<QAction *>(sender());
    QString name = action->text();
    if (name == "通道1-通道4") {
        index = 0;
        videoType = "1_4";
    } else if (name == "通道5-通道8") {
        index = 4;
        videoType = "5_8";
    } else if (name == "通道9-通道12") {
        index = 8;
        videoType = "9_12";
    } else if (name == "通道13-通道16") {
        index = 12;
        videoType = "13_16";
    }
    if (this->video_type_ != videoType) {
        this->video_type_ = videoType;
        ChangeVideo4(index);
    }
}
void FormShow::ShowVideo6() {
    video_max_ = false;
    QString videoType;
    int index = 0;
    QAction *action = static_cast<QAction *>(sender());
    QString name = action->text();
    if (name == "通道1-通道6") {
        index = 0;
        videoType = "1_6";
    } else if (name == "通道6-通道11") {
        index = 5;
        videoType = "6_11";
    } else if (name == "通道11-通道16") {
        index = 10;
        videoType = "11_16";
    }
    if (this->video_type_ != videoType) {
        this->video_type_ = videoType;
        ChangeVideo6(index);
    }
}
void FormShow::ShowVideo8() {
    video_max_ = false;
    QString videoType;
    int index = 0;
    QAction *action = static_cast<QAction *>(sender());
    QString name = action->text();
    if (name == "通道1-通道8") {
        index = 0;
        videoType = "1_8";
    } else if (name == "通道9-通道16") {
        index = 8;
        videoType = "9_16";
    }
    if (this->video_type_ != videoType) {
        this->video_type_ = videoType;
        ChangeVideo8(index);
    }
}
void FormShow::ShowVideo9() {
    video_max_ = false;
    QString videoType;
    int index = 0;
    QAction *action = static_cast<QAction *>(sender());
    QString name = action->text();
    if (name == "通道1-通道9") {
        index = 0;
        videoType = "1_9";
    } else if (name == "通道8-通道16") {
        index = 7;
        videoType = "8_16";
    }
    if (this->video_type_ != videoType) {
        this->video_type_ = videoType;
        ChangeVideo9(index);
    }
}
void FormShow::ShowVideo16() {
    video_max_ = false;
    QString videoType;
    int index = 0;
    videoType = "1_16";
    if (this->video_type_ != videoType) {
        this->video_type_ = videoType;
        ChangeVideo16(index);
    }
}

//---------------------界面清空---------------------//
void FormShow::HideVideoAll() {
    for (int i = 0; i < video_count_; i++) {
        ui->formshow_gridLayout->removeWidget(widgets_.at(i));
        widgets_.at(i)->setVisible(false);
    }
}

//---------------------界面选择---------------------//
void FormShow::ChangeVideo(const qint32 index, const qint32 flag) {
    int count = 0;
    int row = 0;
    int column = 0;
    for (int i = 0; i < video_count_; i++) {
        if (i >= index) {
            ui->formshow_gridLayout->addWidget(widgets_.at(i), row, column);
            widgets_.at(i)->setVisible(true);
            count++;
            column++;
            if (column == flag) {
                row++;
                column = 0;
            }
        }
        if (count == (flag * flag)) {
            break;
        }
    }
}
void FormShow::ChangeVideo4(const qint32 index) {
    HideVideoAll();
    ChangeVideo(index, 2);
}
void FormShow::ChangeVideo6(const qint32 index) {
    HideVideoAll();
    if (index == 0) {
        ui->formshow_gridLayout->addWidget(widgets_.at(0), 0, 0, 2, 2);
        ui->formshow_gridLayout->addWidget(widgets_.at(1), 0, 2, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(2), 1, 2, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(3), 2, 2, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(4), 2, 1, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(5), 2, 0, 1, 1);
        for (int i = 0; i < 6; i++) {
            widgets_.at(i)->setVisible(true);
        }
    } else if (index == 5) {
        ui->formshow_gridLayout->addWidget(widgets_.at(5), 0, 0, 2, 2);
        ui->formshow_gridLayout->addWidget(widgets_.at(6), 0, 2, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(7), 1, 2, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(8), 2, 2, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(9), 2, 1, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(10), 2, 0, 1, 1);
        for (int i = 5; i < 11; i++) {
            widgets_.at(i)->setVisible(true);
        }
    } else if (index == 10) {
        ui->formshow_gridLayout->addWidget(widgets_.at(10), 0, 0, 2, 2);
        ui->formshow_gridLayout->addWidget(widgets_.at(11), 0, 2, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(12), 1, 2, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(13), 2, 2, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(14), 2, 1, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(15), 2, 0, 1, 1);
        for (int i = 10; i < 16; i++) {
            widgets_.at(i)->setVisible(true);
        }
    }
}
void FormShow::ChangeVideo8(const qint32 index) {
    HideVideoAll();
    if (index == 0) {
        ui->formshow_gridLayout->addWidget(widgets_.at(0), 0, 0, 3, 3);
        ui->formshow_gridLayout->addWidget(widgets_.at(1), 0, 3, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(2), 1, 3, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(3), 2, 3, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(4), 3, 3, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(5), 3, 2, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(6), 3, 1, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(7), 3, 0, 1, 1);
        for (int i = 0; i < 8; i++) {
            widgets_.at(i)->setVisible(true);
        }
    } else if (index == 8) {
        ui->formshow_gridLayout->addWidget(widgets_.at(8), 0, 0, 3, 3);
        ui->formshow_gridLayout->addWidget(widgets_.at(9), 0, 3, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(10), 1, 3, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(11), 2, 3, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(12), 3, 3, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(13), 3, 2, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(14), 3, 1, 1, 1);
        ui->formshow_gridLayout->addWidget(widgets_.at(15), 3, 0, 1, 1);
        for (int i = 8; i < 16; i++) {
            widgets_.at(i)->setVisible(true);
        }
    }
}
void FormShow::ChangeVideo9(const qint32 index) {
    HideVideoAll();
    ChangeVideo(index, 3);
}
void FormShow::ChangeVideo16(const qint32 index) {
    HideVideoAll();
    ChangeVideo(index, 4);
}

