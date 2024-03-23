#ifndef TITLETOOLBAR_H
#define TITLETOOLBAR_H

#include <QIcon>
#include <QToolBar>

class TitleToolBar : public QToolBar
{
  Q_OBJECT
      public:
               explicit TitleToolBar(QWidget* p_parent = nullptr);

  TitleToolBar(const QString& p_title, QWidget* p_parent = nullptr);

  void AddTitleBarIcons(const QIcon& p_minimize_icon,
                        const QIcon& p_maximize_icon,
                        const QIcon& p_restore_icon,
                        const QIcon& p_close_icon);

  QAction* AddActionOnLeft(const QIcon& icon,const QString& name);

  void UpdateMaximizeAct();

private:
  void SetupUI();
  void MaximizeRestoreWindow();

private:
  QWidget* window_;
  QAction *maximize_act_, *minimize_act_, *hide_act_;

  QIcon maximize_icon_;
  QIcon restore_icon_;
};

#endif // VTOOLBAR_H
