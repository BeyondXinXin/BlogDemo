#ifndef FORMTITLE_H
#define FORMTITLE_H

/*
 * 显示ui框架
 */

// 01 Frame includes
#include "stable.h"
#include "quimenu.h"

namespace Ui {
    class FormTitle;
}
class QUIMenu;

class FormTitle : public QDialog {
    Q_OBJECT

  public:
    explicit FormTitle(QWidget *parent = nullptr);
    ~FormTitle();
  public slots:
    void SlotMinClicked();
    void SlotMaxClicked();
    void SlotCloseClicked();
    void SlotFullScreen();//全屏
    void SlotNormalScreen();//退出全屏
  Q_SIGNALS:
    void SignalOpenFileOut();
  protected:
    bool eventFilter(QObject *obj, QEvent *evt);
  private :
    void Initial();
  private:
    Ui::FormTitle *ui;
    bool full_screen_;// 是否全屏标识符
    QUIMenu *my_menu_;
};

#endif // FORMTITLE_H
