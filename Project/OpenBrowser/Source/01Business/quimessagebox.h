#ifndef QUIMESSAGEBOX_H
#define QUIMESSAGEBOX_H

/*
 * 弹出信息框类
*/

// 01 Frame includes
#include "stable.h"

class QUIMessageBox : public QDialog {
    Q_OBJECT

  public:
    static QUIMessageBox *Instance();
    explicit QUIMessageBox(QWidget *parent = nullptr);
    ~QUIMessageBox();


  public Q_SLOTS:
    void setIconMain(const QChar &str, quint32 size = 12);
    void setMessage(const QString &msg, int type, int closeSec = 0);


  private Q_SLOTS:
    void SlotBtnOkClicked();
    void SlotBtnMenuCloseClicked();

  protected:
    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *obj, QEvent *evt);
  private:
    void InitControl();             //初始化控件
    void InitForm();                //初始化窗体
    void CheckSec();                //校验倒计时

  private:
    static QScopedPointer<QUIMessageBox> self_;
    qint32 closeSec;                   //总显示时间
    qint32 currentSec;                 //当前已显示时间

  private:
    QVBoxLayout *verticalLayout1;
    QWidget *widgetTitle;
    QHBoxLayout *horizontalLayout3;
    QLabel *labIco;
    QLabel *labTitle;
    QLabel *labTime;
    QWidget *widgetMenu;
    QHBoxLayout *horizontalLayout4;
    QPushButton *btnMenu_Close;
    QWidget *widgetMain;
    QVBoxLayout *verticalLayout2;
    QFrame *frame;
    QVBoxLayout *verticalLayout4;
    QHBoxLayout *horizontalLayout1;
    QLabel *labIcoMain;
    QSpacerItem *horizontalSpacer1;
    QLabel *labInfo;
    QHBoxLayout *horizontalLayout2;
    QSpacerItem *horizontalSpacer2;
    QPushButton *btnOk;
    QPushButton *btnCancel;


};


#endif // QUIMESSAGEBOX_H
