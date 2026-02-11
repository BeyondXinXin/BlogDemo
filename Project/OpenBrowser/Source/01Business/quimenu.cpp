// 01 Frame includes
#include "quimenu.h"
#include "quihelper.h"

// 02 CustomControl includes
#include "screenwidget.h"

// 04 Ui includes
#include "formtitle.h"

QUIMenu::QUIMenu(QWidget *parent, FormTitle *p)
    : QMenuBar(parent) {
    main_window_ = static_cast<FormTitle *>(p) ;
    Initial();
}
QUIMenu::~QUIMenu() {
}
//文件菜单
void QUIMenu::InitialMenuFile() {
    QAction *act_file_new = new QAction(QIcon(":/Image/New.png"),
                                        tr("新建"), this);
    act_file_new->setShortcut(Qt::Key_Control & Qt::Key_I);
    act_file_new->setStatusTip(("新建"));
    QAction *act_file_open = new QAction(QIcon(":/Image/Open.png"),
                                         tr("打开"), this);
    act_file_open->setShortcuts(QKeySequence::Open);
    act_file_open->setStatusTip(("打开"));
    connect(act_file_open, SIGNAL(triggered()),
            main_window_, SIGNAL(SignalOpenFileOut()));
    QAction *act_file_save = new QAction(QIcon(":/Image/Save.png"),
                                         tr("保存"), this);
    act_file_save->setShortcuts(QKeySequence::Save);
    act_file_save->setStatusTip(("保存"));
    QAction *Act_file_saveas = new QAction(QIcon(":/Image/SaveAs.png"),
                                           tr("另存为"), this);
    act_file_new->setShortcut(Qt::Key_Control & Qt::Key_D);
    Act_file_saveas->setStatusTip(("另存为"));
    connect(Act_file_saveas, SIGNAL(triggered()), this, SLOT(SlotFileOpenDatabase()));
    QAction *act_file_close = new QAction(QIcon(":/Image/Close.png"),
                                          tr("关闭"), this);
    act_file_close->setShortcuts(QKeySequence::Close);
    act_file_close->setStatusTip(("关闭软件"));
    QMenu *file = addMenu(("文件"));
    file->addAction(act_file_new);
    file->addAction(act_file_open);
    file->addAction(act_file_save);
    file->addAction(Act_file_saveas);
    file->addSeparator();						//添加一个分割器
    file->addAction(act_file_close);
    toolbar_->addAction(act_file_open);
    toolbar_->addAction(act_file_save);
}
//编辑菜单
void QUIMenu::InitialMenuEdit() {
    QAction *act_edit_full = new QAction(QIcon(":/Image/Edit_Full.png"),
                                         tr("全屏显示"), this);
    act_edit_full->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
    act_edit_full->setStatusTip(("全屏显示"));
    connect(act_edit_full, SIGNAL(triggered()), this, SLOT(SlotQappFullScreen()));
    QAction *act_edit_back = new QAction(QIcon(":/Image/Edit_Max.png"),
                                         tr("退出全屏"), this);
    act_edit_back->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(act_edit_back, SIGNAL(triggered()), this, SLOT(SlotQappMaxScreen()));
    act_edit_back->setStatusTip(("退出全屏"));
    QAction *act_edit_screenshot = new QAction(QIcon(":/Image/btndo2.png"),
            tr("截图"), this);
    act_edit_screenshot->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
    connect(act_edit_screenshot, SIGNAL(triggered()), this, SLOT(SlotSscreenshot()));
    act_edit_screenshot->setStatusTip(("截图"));
    QAction *act_edit_net1 = new QAction(QIcon(""), tr("打开网络——tcp服务器"), this);
    connect(act_edit_net1, SIGNAL(triggered()), this, SLOT(SlotQappNet()));
    QAction *act_edit_net2 = new QAction(QIcon(""), tr("打开网络——tcp客户端"), this);
    connect(act_edit_net2, SIGNAL(triggered()), this, SLOT(SlotQappNet()));
    QAction *act_edit_net3 = new QAction(QIcon(""), tr("打开网络——udp客户端"), this);
    connect(act_edit_net3, SIGNAL(triggered()), this, SLOT(SlotQappNet()));
    act_edit_net1->setObjectName("Act_edit_net1");
    act_edit_net2->setObjectName("Act_edit_net2");
    act_edit_net3->setObjectName("Act_edit_net3");
    act_edit_net1->setStatusTip(("打开网络——tcp服务器"));
    act_edit_net2->setStatusTip(("打开网络——tcp客户端"));
    act_edit_net3->setStatusTip(("打开网络——udp客户端"));
    QAction *act_edit_com = new QAction(QIcon(""), tr("打开端口"), this);
    act_edit_com->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
    connect(act_edit_com, SIGNAL(triggered()), this, SLOT(SlotQappCom()));
    act_edit_com->setStatusTip(("打开端口"));
    QMenu *act_edit_net = new QMenu(("打开网络"));
    act_edit_net->addAction(act_edit_net1);
    act_edit_net->addAction(act_edit_net2);
    act_edit_net->addAction(act_edit_net3);
    act_edit_net->setStatusTip(("打开网络"));
    QMenu *edit = addMenu(("编辑"));
    edit->addAction(act_edit_full);
    edit->addAction(act_edit_back);
    edit->addAction(act_edit_screenshot);
    edit->addSeparator();
    edit->addMenu(act_edit_net);
    edit->addAction(act_edit_com);
    toolbar_->addAction(act_edit_screenshot);
    toolbar_->insertSeparator(act_edit_screenshot);
}

//帮助菜单
void QUIMenu::InitialMenuHelp() {
    QAction *act_about_info =
        new QAction(QIcon(":/ImageProcessing/Image/Help_Info.png"),
                    tr("关于"), this);
    connect(act_about_info, SIGNAL(triggered()), this, SLOT(SlotAboutIinfo()));
    act_about_info->setStatusTip(("关于"));
    QAction *Aat_help_info =
        new QAction(QIcon(":/ImageProcessing/Image/Help_Info.png"),
                    tr("帮助"), this);
    connect(Aat_help_info, SIGNAL(triggered()), this, SLOT(SlotHelpIinfo()));
    Aat_help_info->setStatusTip(("帮助"));
    Aat_help_info->setStatusTip(("帮助"));
    QMenu *help = addMenu(("帮助"));
    help->addAction(act_about_info);
    help->addAction(Aat_help_info);
}


// -------------------槽函数-------------------------------------------
void QUIMenu::SlotSscreenshot() { //截屏
    ScreenWidget::Instance()->showFullScreen();
}
void QUIMenu::SlotQappClose() { // 退出
    qApp->quit();
}
void QUIMenu::SlotFileopen() {	// 打开图片
    QString path = QFileDialog::getOpenFileName(this,
                   tr("选择图像"), ".",
                   tr("Images(*.jpg *.png *.bmp)"));
    if (path.size() == 0) {
        return;
    }
    QTextCodec *code = QTextCodec::codecForName("GB2312");//解决中文路径问题
    std::string name = code->fromUnicode(path).data();
    QUIHelper::ShowMessageBoxInfo(QString("打开图片路径:%1").arg(path));
    QUIHelper::ShowMessageBoxQuestion("图片和数据库无法匹配打开失败");
}
void QUIMenu::SlotFilesave() {		// 保存文件
}
void QUIMenu::SlotFileOpenDatabase() {		// 打开配置文件
    QString path = QFileDialog::getOpenFileName(this,
                   tr("选择图像"), ".",
                   tr("Images(*.ini)"));
    if (path.size() == 0) {
        return;
    }
    QTextCodec *code = QTextCodec::codecForName("GB2312");//解决中文路径问题
    std::string name = code->fromUnicode(path).data();
    QUIHelper::ShowMessageBoxInfo(QString("打开图片路径:%1").arg(path));
    QUIHelper::ShowMessageBoxQuestion("图片和数据库无法匹配打开失败");
}
void QUIMenu::SlotQappFullScreen() { //全屏
    main_window_->showFullScreen();
}
void QUIMenu::SlotQappMaxScreen() { //退出全屏
    main_window_->showNormal();
}

void QUIMenu::SlotQappNet() { //网络
    QString a = sender()->objectName();
    QString b = QUIHelper::GetLocalIP();
    if (a == "Act_edit_net1") {
        frm_tcp_client_->setWindowTitle("打开网络——tcp服务器    本机ip:" + b);
        frm_tcp_client_->show();
    } else if (a == "Act_edit_net2") {
        frm_tcp_server_->setWindowTitle("打开网络——tcp客户端    本机ip:" + b);
        frm_tcp_server_->show();
    } else if (a == "Act_edit_net3") {
        frm_udpserver_->setWindowTitle("打开网络——udp客户端    本机ip:" + b);
        frm_udpserver_->show();
    }
}

void QUIMenu::SlotQappCom() { //打开串口
    com_Settings_->show();
}

QToolBar *QUIMenu::GetToolBar() {
    return this->toolbar_;
}

void QUIMenu::Initial() {
    frm_tcp_client_ = new frmTcpClient();
    frm_tcp_server_ = new frmTcpServer();
    frm_udpserver_ = new frmUdpServer();
    com_Settings_  = new SettingsDialog();
    toolbar_ = new QToolBar(("工具栏01")) ;
    toolbar_->setObjectName("toolbar");
    Qt::ToolBarArea area;
    area = Qt::LeftToolBarArea;//设置停靠位置
    toolbar_-> setMovable(true);
    toolbar_-> setFixedWidth(300);
    InitialMenuFile();
    InitialMenuEdit();
    InitialMenuHelp();
    this->setFixedWidth(200);
}


// -------------------帮助-------------------------------------------
void QUIMenu::SlotHelpIinfo() {
}
// -------------------关于-------------------------------------------
void QUIMenu::SlotAboutIinfo() {
}

