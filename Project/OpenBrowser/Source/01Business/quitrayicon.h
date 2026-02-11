#ifndef QUITRAYICON_H
#define QUITRAYICON_H

/*
 * 缩略图标类
*/

// 01 Frame includes
#include "stable.h"

//托盘图标类
class QUITrayIcon : public QObject {
    Q_OBJECT
  public:
    static QUITrayIcon *Instance();
    explicit QUITrayIcon(QObject *parent = nullptr);
    void SetExitDirect(bool exit_direct_); // 设置是否直接退出,如果不是直接退出则发送信号给主界面
    void SetMainWidget(QWidget *main_widget_);// 设置所属主窗体
    void ShowMessage(const QString &title, const QString &msg,
                     QSystemTrayIcon::MessageIcon
                     icon = QSystemTrayIcon::Information, int msecs = 5000);  // 显示消息
    void SetIcon(const QString &strIcon);// 设置图标
    void ToolTip(const QString &tip);  // 设置提示信息
    void SlotsetVisible(bool visible); // 设置是否可见
  public Q_SLOTS:
  Q_SIGNALS:
    void TrayIconExit();
  private slots:
    void SlotIconIsActived(QSystemTrayIcon::ActivationReason reason);
    void SlotCloseAll();  // 退出所有
  private:
    static QScopedPointer<QUITrayIcon> self_;
    QWidget *main_widget_;            //对应所属主窗体
    QSystemTrayIcon *tray_icon_;      //托盘对象
    QMenu *menu_;                    //右键菜单
    bool exit_direct_;                //是否直接退出
};

#endif // QUITRAYICON_H
