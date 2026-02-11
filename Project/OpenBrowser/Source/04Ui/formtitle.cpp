// 01 Frame includes
#include "formtitle.h"
#include "quiiconhelper.h"
#include "ui_formtitle.h"


FormTitle::FormTitle(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormTitle) {
    ui->setupUi(this);
    this->Initial();
}

FormTitle::~FormTitle() {
    delete ui;
}


void FormTitle::Initial() {
    full_screen_ = false;
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setProperty("can_move", 1);
    this->setSizeGripEnabled(true);
    this->setMinimumSize(1200, 900);
    ui->widgetTitle->installEventFilter(this);
    this->my_menu_ = new QUIMenu(this, this);
    ui->layout_titlemenu->addWidget(my_menu_);
    ui->layout_titlemenu->addWidget(my_menu_->GetToolBar());
    connect(this, SIGNAL(SignalOpenFileOut()),
            ui->main_widget, SLOT(SlotOpenFileIn()));
    QUIIconHelper::Instance()->SetIcon(ui->btnmenu_min, QChar(0xf068));
    QUIIconHelper::Instance()->SetIcon(ui->btnmenu_max, QChar(0xf2d0));
    QUIIconHelper::Instance()->SetIcon(ui->btnmenu_close, QChar(0xf00d));
    connect(ui->btnmenu_close, &QPushButton::clicked, this, &FormTitle::SlotCloseClicked);
    connect(ui->btnmenu_max, &QPushButton::clicked, this, &FormTitle::SlotMaxClicked);
    connect(ui->btnmenu_min, &QPushButton::clicked, this, &FormTitle::SlotMinClicked);
}


void FormTitle::SlotMinClicked() {
    showMinimized();
}

void FormTitle::SlotMaxClicked() {
    static bool max = false;
    static QRect location = this->geometry();
    if (max) {
        this->setGeometry(location);
        QUIIconHelper::Instance()->SetIcon(ui->btnmenu_max, QChar(0xf2d0));
    } else {
        location = this->geometry();
        this->setGeometry(qApp->desktop()->availableGeometry());
        QUIIconHelper::Instance()->SetIcon(ui->btnmenu_max, QChar(0xf2d2));
    }
    this->setProperty("can_move", max);
    max = !max;
}

void FormTitle::SlotCloseClicked() {
    this->close();
}

void FormTitle::SlotFullScreen() {
    ui->widget_titlebtn->hide();
    full_screen_ = true;
    this->showFullScreen();
}

void FormTitle::SlotNormalScreen() {
    ui->widget_titlebtn->show();
    full_screen_ = false;
    this->showNormal();
}

bool FormTitle::eventFilter(QObject *obj, QEvent *evt) {
    if (false == full_screen_) {
        QMouseEvent *event = static_cast<QMouseEvent *>(evt);
        if (event->type() == QEvent::MouseButtonDblClick) {
            SlotMaxClicked();
        }
    }
    return QWidget::eventFilter(obj, evt);
}


