#include "title_tool_bar.h"

#include <QLayout>
#include <QToolButton>

TitleToolBar::TitleToolBar(QWidget* p_parent)
    : QToolBar(p_parent)
      , window_(p_parent)
{
  SetupUI();
}

TitleToolBar::TitleToolBar(const QString& p_title, QWidget* p_parent)
    : QToolBar(p_title, p_parent)
      , window_(p_parent)
{
  SetupUI();
}

void TitleToolBar::SetupUI()
{
  auto spacer = new QWidget(this);
  spacer->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  auto act = addWidget(spacer);
  act->setEnabled(false);
  setMovable(false);
  // addSeparator();
}

void TitleToolBar::MaximizeRestoreWindow()
{
  window_->isMaximized() ? window_->showNormal() : window_->showMaximized();
}

void TitleToolBar::AddTitleBarIcons(const QIcon& p_minimize_icon,
                                    const QIcon& p_maximize_icon,
                                    const QIcon& p_restore_icon,
                                    const QIcon& p_close_icon)
{

  minimize_act_ = addAction(p_minimize_icon, tr("Minimize"),
                            this, [this]() {
                              window_->showMinimized();
                            });
  maximize_icon_ = p_maximize_icon;
  restore_icon_ = p_restore_icon;
  maximize_act_ = addAction(p_maximize_icon, tr("Maximize"),
                            this, [this]() {
                              MaximizeRestoreWindow();
                            });
  hide_act_ = addAction(p_close_icon, tr("Close"),
                        this, [this]() {
                          window_->hide();
                        });
  UpdateMaximizeAct();
  minimize_act_->setVisible(false);
  maximize_act_->setVisible(false);
}

QAction* TitleToolBar::AddActionOnLeft(const QIcon& icon, const QString& name)
{
  return addAction(icon, name);
}

void TitleToolBar::UpdateMaximizeAct()
{
  if (window_->isMaximized()) {
    maximize_act_->setIcon(restore_icon_);
    maximize_act_->setText(tr("Restore Down"));
  } else {
    maximize_act_->setIcon(maximize_icon_);
    maximize_act_->setText(tr("Maximize"));
  }
}
