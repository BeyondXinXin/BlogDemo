#include "tool_widget.h"

#include "palette_manager.h"

ToolWidget::ToolWidget(QWidget *parent)
  : QToolBar(parent)
{
    setMouseTracking(true);
    setWindowFlags(Qt::WindowCloseButtonHint);
    setOrientation(Qt::Vertical);

    setFloatable(false);
    setMovable(false);
}

ToolWidget::~ToolWidget()
{
}
