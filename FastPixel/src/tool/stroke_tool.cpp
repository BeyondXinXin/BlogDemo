#include "stroke_tool.h"

#include "data/vector2d.h"
#include "editor/edit_scene.h"
#include "editor/edit_view.h"
#include "editor/editor_data.h"

StrokeTool::StrokeTool()
  : Tool()
{
    setToolTip(tr("Stroke"));
    setIcon(QIcon(QPixmap(":/png/stroke.png")));
    cursor_ = QCursor(QPixmap(":/png/stroke.png"), -16, -16);
    use_buffer_ = false;
}

void StrokeTool::HandelPress(EditorData* data, const Cell& cell, const QString& color)
{
    start_cell_ = cell;
    end_cell_ = cell;
    CalculationBufferData(color);

    data->SetBufferData(buffer_data_);
}

void StrokeTool::HandelMove(
  EditorData* data, const Cell& from, const Cell& to, const QString& color)
{
    end_cell_ = to;
    CalculationBufferData(color);

    data->SetBufferData(buffer_data_);
}

void StrokeTool::HandelRelease(EditorData* data, const Cell& cellr)
{
    Q_UNUSED(cellr)

    auto itea = buffer_data_.begin();
    while (itea != buffer_data_.end()) {
        data->ChangePixel(itea.key(), itea.value());
        itea++;
    }

    buffer_data_.clear();
    data->SetBufferData(buffer_data_);
}

void StrokeTool::CalculationBufferData(const QString& color)
{
    buffer_data_.clear();

    Vector2D delta(end_cell_.x - start_cell_.x, end_cell_.y - start_cell_.y);
    double length = delta.Length();
    if (length < 1.2) {
        buffer_data_[start_cell_] = color;
    } else {
        delta.Normalize();
        for (int i = 0; i <= length; i++) {
            const int offset_x = static_cast<int>(delta.X() * i); // 向下取整
            const int offset_y = static_cast<int>(delta.Y() * i); // 向下取整
            const Cell cell(start_cell_.x + offset_x, start_cell_.y + offset_y);
            buffer_data_[cell] = color;
        }
        buffer_data_[end_cell_] = color;
    }
    return;
}
