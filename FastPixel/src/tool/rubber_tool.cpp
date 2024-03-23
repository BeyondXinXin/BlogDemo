#include "rubber_tool.h"
#include "editor_data.h"

#include "data/vector2d.h"

#include <QSpinBox>

RubberTool::RubberTool()
  : Tool()
{
    setToolTip(tr("Rubber"));
    setIcon(QIcon(QPixmap(":/png/rubber_tool.png")));
    cursor_ = QCursor(QPixmap(":/png/rubber_tool.png"), -16, -16);

    real_time_update_ = true;

    size_ = 1;

    InitView();
}

void RubberTool::HandelPress(EditorData* data, const Cell& cell, const QString& color)
{
    Q_UNUSED(color)

    if (size_ < 2) {
        data->ChangePixel(cell, "transparent");
        return;
    }

    const int offset_1 = (size_ - 1) / 2;
    const int offset_2 = size_ / 2;
    QList<Cell> cells = data->Cells(Cell(cell.x - offset_1, cell.y - offset_1),
                                    Cell(cell.x + offset_2, cell.y + offset_2));

    foreach (auto cell, cells) {
        data->ChangePixel(cell, "transparent");
    }
}

void RubberTool::HandelMove(
  EditorData* data, const Cell& from, const Cell& to, const QString& color)
{
    Q_UNUSED(color)
    Vector2D delta(to.x - from.x, to.y - from.y);
    double length = delta.Length();

    if (length < 1.2) {
        HandelPress(data, to, "transparent");
    }

    // size的关系 效率低，待优化
    delta.Normalize();
    for (int i = 0; i <= length; i++) {
        const int offset_x = static_cast<int>(delta.X() * i); // 向下取整
        const int offset_y = static_cast<int>(delta.Y() * i); // 向下取整
        const Cell cell(from.x + offset_x, from.y + offset_y);
        HandelPress(data, cell, "transparent");
    }
}

void RubberTool::HandelRelease(EditorData* data, const Cell& cellr)
{
    Q_UNUSED(data)
    Q_UNUSED(cellr)
}

void RubberTool::InitView()
{
    size_box_ = new QSpinBox();
    connect(size_box_, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &RubberTool::HandleSpinBoxChange);
    size_box_->setFocusPolicy(Qt::NoFocus);

    size_box_->setValue(1);
    size_box_->setRange(1, 10);

    attribute_wids_ << size_box_;
}

void RubberTool::HandleSpinBoxChange(int data)
{
    auto* box = qobject_cast<QSpinBox*>(sender());
    if (box == size_box_) {
        size_ = data;
    }
}
