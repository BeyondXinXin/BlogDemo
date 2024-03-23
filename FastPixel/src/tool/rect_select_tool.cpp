#include "rect_select_tool.h"

#include "data/vector2d.h"
#include "editor_data.h"
#include "layout/layout_buffer.h"

#include <QPushButton>

RectSelectTool::RectSelectTool()
  : Tool()
{
    setToolTip(tr("Pen"));
    setIcon(QIcon(QPixmap(":/png/rect_select.png")));
    cursor_ = QCursor(QPixmap(":/png/rect_select.png"), -16, -16);

    use_buffer_ = true;

    InitView();
}

// 鼠标按下
void RectSelectTool::HandelPress(LayoutBuffer* lay_buffer, const Cell& cell)
{
    if (lay_buffer->Contain(cell)) { // 移动区域
        moveing_ = true;
        first_cell_ = cell;
        lay_buffer->StartMove();
    } else { // 绘制区域
        moveing_ = false;
        lay_buffer->SetFirstCell(cell);
    }
}

// 移动
void RectSelectTool::HandelMove(LayoutBuffer* lay_buffer,
                                const Cell& from,
                                const Cell& to)
{
    Q_UNUSED(from)

    if (moveing_) { // 移动区域
        second_cell_ = to;
        Cell offset = second_cell_ - first_cell_;
        first_cell_ = second_cell_;

        lay_buffer->MoveCell(offset);
    } else { // 绘制区域
        lay_buffer->SetEndCell(to);
    }
}

void RectSelectTool::HandelRelease(EditorData* data, const Cell& cellr)
{
    Q_UNUSED(cellr)

    if (moveing_) {
        data->SaveBufferNoAppendStack();
        moveing_ = false;
    }
}

void RectSelectTool::InitView()
{
    btn_clear_ = new QPushButton();
    btn_clear_->setFocusPolicy(Qt::NoFocus);
    btn_clear_->setText(tr("clear"));
    connect(btn_clear_, &QPushButton::clicked, this, &RectSelectTool::HandlePushButton);

    attribute_wids_ << btn_clear_;
}

void RectSelectTool::HandlePushButton()
{
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (btn == btn_clear_) { // 清空
        emit SgnToolEvent(KRectSelect_Clear);
    }
}
