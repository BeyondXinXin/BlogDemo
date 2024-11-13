#ifndef TOOLMANAGER_H
#define TOOLMANAGER_H

#include "tool.h"

class QSpinBox;
class ToolWidget;

class ToolManager : public QObject
{
    Q_OBJECT
public:
    ToolManager();

    QWidget* GetToolWid();
    Tool* CurToolBar();

Q_SIGNALS:
    void SgnChangeTool();
    void SgnToolEvent(const ToolEvent& data);

private:
    void HandleToolBarActionClick(QAction* action);

private:
    QList<Tool*> tools_;
    Tool* cur_tool_;
    ToolWidget* tool_bar_wid_;
};

#endif // TOOLMANAGER_H
