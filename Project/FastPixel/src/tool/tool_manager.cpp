#include "tool_manager.h"

#include "ellipse_tool.h"
#include "paint_bucket_tool.h"
#include "pen_tool.h"
#include "rect_select_tool.h"
#include "rect_tool.h"
#include "rubber_tool.h"
#include "stroke_tool.h"
#include "tool_widget.h"

#include <QSpinBox>

ToolManager::ToolManager()
  : tool_bar_wid_(new ToolWidget())
{
    // 工具栏-笔
    Tool* tool = new PenTool();
    tool->setShortcut(QKeySequence("1"));
    tools_ << tool;
    // 工具栏-橡皮
    tool = new RubberTool();
    tool->setShortcut(QKeySequence("2"));
    tools_ << tool;
    // 工具栏-矩形
    tool = new RectTool();
    tool->setShortcut(QKeySequence("3"));
    tools_ << tool;
    // 工具栏-椭圆
    tool = new EllipseTool();
    tool->setShortcut(QKeySequence("4"));
    tools_ << tool;
    // 工具栏-油漆桶
    tool = new PaintBucketTool();
    tool->setShortcut(QKeySequence("5"));
    tools_ << tool;
    // 工具栏-线
    tool = new StrokeTool();
    tool->setShortcut(QKeySequence("6"));
    tools_ << tool;
    // 工具栏-矩形选择框
    tool = new RectSelectTool();
    tool->setShortcut(QKeySequence("7"));
    tools_ << tool;

    QActionGroup* action_group = new QActionGroup(this);
    foreach (auto tool, tools_) {
        tool_bar_wid_->addAction(tool);
        action_group->addAction(tool);
        connect(tool, &Tool::SgnToolEvent, this, &ToolManager::SgnToolEvent);
    }
    action_group->setEnabled(true);
    action_group->setExclusive(true);

    // 信号槽
    connect(tool_bar_wid_, &QToolBar::actionTriggered,
            this, &ToolManager::HandleToolBarActionClick);

    tools_[0]->trigger();
}

QWidget* ToolManager::GetToolWid()
{
    return tool_bar_wid_;
}

Tool* ToolManager::CurToolBar()
{
    return cur_tool_;
}

// 切换工具
void ToolManager::HandleToolBarActionClick(QAction* action)
{
    Tool* tool = qobject_cast<Tool*>(action);

    if (cur_tool_ != tool) {
        cur_tool_ = tool;
        // 发射信号
        emit SgnChangeTool();
    }
}
