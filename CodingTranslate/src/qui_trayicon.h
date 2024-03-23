#ifndef QUITRAYICON_H
#define QUITRAYICON_H

#include <QSystemTrayIcon>
#include <QWidget>


//托盘图标类
class QUITrayIcon : public QObject
{
    Q_OBJECT
public:
    static QUITrayIcon* Instance();
    explicit QUITrayIcon(QObject* parent = nullptr);
    void SetExitDirect(bool exit_direct_);
    void SetMainWidget(QWidget* main_widget);
    void ShowMessage(const QString& title, const QString& msg,
                     QSystemTrayIcon::MessageIcon
                       icon = QSystemTrayIcon::Information,
                     int msecs = 5000);
    void SetIcon(const QString& str_icon);
    void SetIcon(const QIcon& icon);
    void ToolTip(const QString& tip);
    void SlotsetVisible(bool visible);
public Q_SLOTS:
Q_SIGNALS:
    void TrayIconExit();
private slots:
    void SlotIconIsActived(QSystemTrayIcon::ActivationReason reason);
    void SlotCloseAll(); // 退出所有
private:
    static QScopedPointer<QUITrayIcon> self_;
    QWidget* main_widget_; //对应所属主窗体
    QSystemTrayIcon* tray_icon_; //托盘对象
    QMenu* menu_; //右键菜单
    bool exit_direct_; //是否直接退出
};

#endif // QUITRAYICON_H
