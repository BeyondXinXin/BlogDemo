// 01 Frame includes
#include "quihelper.h"
#include "quimessagebox.h"
#include "quiiconhelper.h"

QScopedPointer<QUIMessageBox> QUIMessageBox::self_;
QUIMessageBox *QUIMessageBox::Instance() {
    if (self_.isNull()) {
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self_.isNull()) {
            self_.reset(new QUIMessageBox);
        }
    }
    return self_.data();
}

QUIMessageBox::QUIMessageBox(QWidget *parent) : QDialog(parent) {
    this->InitControl();
    this->InitForm();
    QUIHelper::SetFormInCenter(this);
}

QUIMessageBox::~QUIMessageBox() {
    delete widgetMain;
}

void QUIMessageBox::InitControl() {
    this->setObjectName(QString::fromUtf8("QUIMessageBox"));
    verticalLayout1 = new QVBoxLayout(this);
    verticalLayout1->setSpacing(0);
    verticalLayout1->setObjectName(QString::fromUtf8("verticalLayout1"));
    verticalLayout1->setContentsMargins(1, 1, 1, 1);
    widgetTitle = new QWidget(this);
    widgetTitle->setObjectName(QString::fromUtf8("widgetTitle"));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(widgetTitle->sizePolicy().hasHeightForWidth());
    widgetTitle->setSizePolicy(sizePolicy);
    widgetTitle->setMinimumSize(QSize(0, 30));
    horizontalLayout3 = new QHBoxLayout(widgetTitle);
    horizontalLayout3->setSpacing(0);
    horizontalLayout3->setObjectName(QString::fromUtf8("horizontalLayout3"));
    horizontalLayout3->setContentsMargins(0, 0, 0, 0);
    labIco = new QLabel(widgetTitle);
    labIco->setObjectName(QString::fromUtf8("labIco"));
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(labIco->sizePolicy().hasHeightForWidth());
    labIco->setSizePolicy(sizePolicy1);
    labIco->setMinimumSize(QSize(30, 0));
    labIco->setAlignment(Qt::AlignCenter);
    horizontalLayout3->addWidget(labIco);
    labTitle = new QLabel(widgetTitle);
    labTitle->setObjectName(QString::fromUtf8("labTitle"));
    labTitle->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
    horizontalLayout3->addWidget(labTitle);
    labTime = new QLabel(widgetTitle);
    labTime->setObjectName(QString::fromUtf8("labTime"));
    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(labTime->sizePolicy().hasHeightForWidth());
    labTime->setSizePolicy(sizePolicy2);
    labTime->setAlignment(Qt::AlignCenter);
    horizontalLayout3->addWidget(labTime);
    widgetMenu = new QWidget(widgetTitle);
    widgetMenu->setObjectName(QString::fromUtf8("widgetMenu"));
    sizePolicy1.setHeightForWidth(widgetMenu->sizePolicy().hasHeightForWidth());
    widgetMenu->setSizePolicy(sizePolicy1);
    horizontalLayout4 = new QHBoxLayout(widgetMenu);
    horizontalLayout4->setSpacing(0);
    horizontalLayout4->setObjectName(QString::fromUtf8("horizontalLayout4"));
    horizontalLayout4->setContentsMargins(0, 0, 0, 0);
    btnMenu_Close = new QPushButton(widgetMenu);
    btnMenu_Close->setObjectName(QString::fromUtf8("btnMenu_Close"));
    QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Expanding);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(btnMenu_Close->sizePolicy().hasHeightForWidth());
    btnMenu_Close->setSizePolicy(sizePolicy3);
    btnMenu_Close->setMinimumSize(QSize(30, 0));
    btnMenu_Close->setMaximumSize(QSize(30, 16777215));
    btnMenu_Close->setCursor(QCursor(Qt::ArrowCursor));
    btnMenu_Close->setFocusPolicy(Qt::NoFocus);
    btnMenu_Close->setFlat(true);
    horizontalLayout4->addWidget(btnMenu_Close);
    horizontalLayout3->addWidget(widgetMenu);
    verticalLayout1->addWidget(widgetTitle);
    widgetMain = new QWidget(this);
    widgetMain->setObjectName(QString::fromUtf8("widgetMainQUI"));
    verticalLayout2 = new QVBoxLayout(widgetMain);
    verticalLayout2->setSpacing(5);
    verticalLayout2->setObjectName(QString::fromUtf8("verticalLayout2"));
    verticalLayout2->setContentsMargins(5, 5, 5, 5);
    frame = new QFrame(widgetMain);
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Sunken);
    verticalLayout4 = new QVBoxLayout(frame);
    verticalLayout4->setObjectName(QString::fromUtf8("verticalLayout4"));
    verticalLayout4->setContentsMargins(-1, 9, -1, -1);
    horizontalLayout1 = new QHBoxLayout();
    horizontalLayout1->setObjectName(QString::fromUtf8("horizontalLayout1"));
    labIcoMain = new QLabel(frame);
    labIcoMain->setObjectName(QString::fromUtf8("labIcoMain"));
    labIcoMain->setAlignment(Qt::AlignCenter);
    horizontalLayout1->addWidget(labIcoMain);
    horizontalSpacer1 = new QSpacerItem(5, 0,
                                        QSizePolicy::Minimum, QSizePolicy::Minimum);
    horizontalLayout1->addItem(horizontalSpacer1);
    labInfo = new QLabel(frame);
    labInfo->setObjectName(QString::fromUtf8("labInfo"));
    QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(labInfo->sizePolicy().hasHeightForWidth());
    labInfo->setSizePolicy(sizePolicy4);
    labInfo->setMinimumSize(QSize(0, 30));
    labInfo->setScaledContents(false);
    labInfo->setWordWrap(true);
    horizontalLayout1->addWidget(labInfo);
    verticalLayout4->addLayout(horizontalLayout1);
    horizontalLayout2 = new QHBoxLayout();
    horizontalLayout2->setObjectName(QString::fromUtf8("horizontalLayout2"));
    horizontalSpacer2 = new QSpacerItem(
        40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout2->addItem(horizontalSpacer2);
    btnOk = new QPushButton(frame);
    btnOk->setObjectName(QString::fromUtf8("btnOk"));
    btnOk->setMinimumSize(QSize(85, 0));
    btnOk->setFocusPolicy(Qt::StrongFocus);
    btnOk->setIcon(QIcon(":/Image/btn_ok.png"));
    horizontalLayout2->addWidget(btnOk);
    btnCancel = new QPushButton(frame);
    btnCancel->setObjectName(QString::fromUtf8("btnCancel"));
    btnCancel->setMinimumSize(QSize(85, 0));
    btnCancel->setFocusPolicy(Qt::StrongFocus);
    btnCancel->setIcon(QIcon(":/Image/btn_close.png"));
    horizontalLayout2->addWidget(btnCancel);
    verticalLayout4->addLayout(horizontalLayout2);
    verticalLayout2->addWidget(frame);
    verticalLayout1->addWidget(widgetMain);
    widgetTitle->raise();
    widgetMain->raise();
    frame->raise();
    btnOk->setText("确定");
    btnCancel->setText("取消");
    connect(btnOk, SIGNAL(clicked()), this, SLOT(SlotBtnOkClicked()));
    connect(btnMenu_Close, SIGNAL(clicked()),
            this, SLOT(SlotBtnMenuCloseClicked()));
    connect(btnCancel, SIGNAL(clicked()),
            this, SLOT(SlotBtnMenuCloseClicked()));
}

void QUIMessageBox::InitForm() {
    QUIIconHelper::Instance()->SetIcon(
        labIco, QChar(0xf072), 14);
    QUIIconHelper::Instance()->SetIcon(
        btnMenu_Close, QChar(0xf00d), 12);
    this->setProperty("form", true);
    this->widgetTitle->setProperty("form", "title");
    this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    this->setWindowTitle(this->labTitle->text());
    int width = 80;
    int iconWidth = 18;
    int iconHeight = 18;
    this->setFixedSize(280, 150);
    labIcoMain->setFixedSize(30, 30);
    QList<QPushButton *> btns = this->frame->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        btn->setMinimumWidth(width);
        btn->setIconSize(QSize(iconWidth, iconHeight));
    }
    closeSec = 0;
    currentSec = 0;
    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(CheckSec()));
    timer->start();
    this->installEventFilter(this);
}

void QUIMessageBox::CheckSec() {
    if (closeSec == 0) {
        return;
    }
    if (currentSec < closeSec) {
        currentSec++;
    } else {
        this->close();
    }
    QString str = QString("关闭倒计时 %1 s").arg(closeSec - currentSec + 1);
    this->labTime->setText(str);
}

void QUIMessageBox::setMessage(const QString &msg, int type, int closeSec) {
    this->setMinimumWidth(0);
    this->closeSec = closeSec;
    this->currentSec = 0;
    this->labTime->clear();
    CheckSec();
    //图片存在则取图片,不存在则取图形字体
    int size = this->labIcoMain->size().height();
    bool exist = !QImage(":/Image/msg_info.png").isNull();
    if (type == 0) {
        if (exist) {
            this->labIcoMain->
            setStyleSheet("border-image: url(:/Image/msg_info.png);");
        } else {
            QUIIconHelper::Instance()->SetIcon(
                this->labIcoMain, 0xf05a, static_cast<quint32>(size));
        }
        this->btnCancel->setVisible(false);
        this->labTitle->setText("提示");
    } else if (type == 1) {
        if (exist) {
            this->labIcoMain->
            setStyleSheet("border-image: url(:/Image/msg_question.png);");
        } else {
            QUIIconHelper::Instance()->SetIcon(
                this->labIcoMain, 0xf059, static_cast<quint32>(size));
        }
        this->setMinimumWidth(1200);
        this->labTitle->setText("询问");
    } else if (type == 2) {
        if (exist) {
            this->labIcoMain->
            setStyleSheet("border-image: url(:/Image/msg_error.png);");
        } else {
            QUIIconHelper::Instance()->SetIcon(
                this->labIcoMain, 0xf057, static_cast<quint32>(size));
        }
        this->btnCancel->setVisible(false);
        this->labTitle->setText("错误");
    }
    this->labInfo->setText(msg);
    this->setWindowTitle(this->labTitle->text());
}

void QUIMessageBox::closeEvent(QCloseEvent *) {
    closeSec = 0;
    currentSec = 0;
}

bool QUIMessageBox::eventFilter(QObject *obj, QEvent *evt) {
    static QPoint mousePoint;
    static bool mousePressed = false;
    QMouseEvent *event = static_cast<QMouseEvent *>(evt);
    if (event->type() == QEvent::MouseButtonPress) {
        if (event->button() == Qt::LeftButton) {
            mousePressed = true;
            mousePoint = event->globalPos() - this->pos();
            return true;
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        mousePressed = false;
        return true;
    } else if (event->type() == QEvent::MouseMove) {
        if (mousePressed && (event->buttons() && Qt::LeftButton)) {
            this->move(event->globalPos() - mousePoint);
            return true;
        }
    }
    return QWidget::eventFilter(obj, evt);
}

void QUIMessageBox::SlotBtnOkClicked() {
    done(QMessageBox::Yes);
    close();
}

void QUIMessageBox::SlotBtnMenuCloseClicked() {
    done(QMessageBox::No);
    close();
}

void QUIMessageBox::setIconMain(const QChar &str, quint32 size) {
    QUIIconHelper::Instance()->SetIcon(this->labIco, str, size);
}


