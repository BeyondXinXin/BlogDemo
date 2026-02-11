#ifndef MYSCROLLAREA_H
#define MYSCROLLAREA_H

/*
 * 菜单栏、状态栏
*/

// 01 Frame includes
#include "stable.h"

// 02 CustomControl includes
#include "frmtcpclient.h"
#include "settingsdialog.h"
#include "frmtcpserver.h"
#include "frmudpserver.h"


class FormTitle;

class QUIMenu : public QMenuBar {
    Q_OBJECT
  public:
    QUIMenu(QWidget *parent, FormTitle *p);
    ~QUIMenu();
  public slots :
    // 界面目录
    void SlotFileopen();		// 打开
    void SlotFilesave();		// 保存
    void SlotFileOpenDatabase();		// 另存为
    void SlotHelpIinfo();		// 帮助
    void SlotAboutIinfo();//关于
    void SlotSscreenshot();//截图
    void SlotQappClose();//退出
    void SlotQappFullScreen();//全屏
    void SlotQappMaxScreen();//退出全屏
    void SlotQappNet();//打开网络
    void SlotQappCom();//打开串口
    QToolBar *GetToolBar();



  private:
    void Initial();
    void InitialMenuFile();		// 文件菜单
    void InitialMenuEdit();		// 编辑菜单
    void InitialMenuHelp();		// 帮助菜单
  private:
    QToolBar *toolbar_;
    FormTitle *main_window_;
    frmTcpClient *frm_tcp_client_;
    frmTcpServer *frm_tcp_server_;
    frmUdpServer *frm_udpserver_;
    QSerialPort *serial_port_; //串口类
    SettingsDialog *com_Settings_;
};


#endif // MYSCROLLAREA_H
