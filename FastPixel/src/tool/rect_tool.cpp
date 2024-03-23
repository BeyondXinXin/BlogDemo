#include "rect_tool.h"

#include "edit_scene.h"
#include "edit_view.h"
#include "editor_data.h"

RectTool::RectTool()
  : Tool()
{
    setToolTip(tr("Pen"));
    setIcon(QIcon(QPixmap(":/png/rect.png")));
    cursor_ = QCursor(QPixmap(":/png/rect.png"), -16, -16);

    use_buffer_ = false;
}

void RectTool::HandelPress(EditorData* data, const Cell& cell, const QString& color)
{
    left_up_ = cell;
    right_down_ = cell;
    CalculationBufferData(color);

    data->SetBufferData(buffer_data_);
}

void RectTool::HandelMove(
  EditorData* data, const Cell& from, const Cell& to, const QString& color)
{
    right_down_ = to;
    CalculationBufferData(color);

    data->SetBufferData(buffer_data_);
}

void RectTool::HandelRelease(EditorData* data, const Cell& cellr)
{
    Q_UNUSED(cellr)
    data->SaveBuffer();
    buffer_data_.clear();
}

void RectTool::CalculationBufferData(const QString& color)
{
    buffer_data_.clear();

    const int offset_x = left_up_.x > right_down_.x ? -1 : 1;
    const int offset_y = left_up_.y > right_down_.y ? -1 : 1;
    const int X = right_down_.x - left_up_.x;
    const int Y = right_down_.y - left_up_.y;

    for (int x = 0; x != X; x += offset_x) {
        buffer_data_[Cell(left_up_.x + x, left_up_.y)] = color;
        buffer_data_[Cell(left_up_.x + x, left_up_.y + Y)] = color;
    }

    for (int y = 0; y != Y; y += offset_y) {
        buffer_data_[Cell(left_up_.x, left_up_.y + y)] = color;
        buffer_data_[Cell(left_up_.x + X, left_up_.y + y)] = color;
    }
    buffer_data_[right_down_] = color;
}
