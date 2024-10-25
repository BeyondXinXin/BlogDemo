#ifndef TOOL_WIDGET_H
#define TOOL_WIDGET_H

#include <QToolBar>

class ToolWidget : public QToolBar
{
    Q_OBJECT
public:
    ToolWidget(QWidget* parent = nullptr);
    ~ToolWidget() override;
};

#endif // PALETE_WIDGET_H
