#include "pen_tool.h"

#include "data/vector2d.h"
#include "editor/editor_data.h"

#include <QSpinBox>

PenTool::PenTool()
  : Tool()
{
    setToolTip(tr("Pen"));
    setIcon(QIcon(QPixmap(":/png/pen_tool.png")));
    cursor_ = QCursor(QPixmap(":/png/pen_tool.png"), -16, -16);

    size_ = 1;

    InitView();
}

void PenTool::HandelPress(EditorData* data, const Cell& cell, const QString& color)
{

    if (size_ < 2) {
        data->ChangeBufferCell(cell, color);
        return;
    }

    const int offset_1 = (size_ - 1) / 2;
    const int offset_2 = size_ / 2;
    QList<Cell> cells = data->Cells(Cell(cell.x - offset_1, cell.y - offset_1),
                                    Cell(cell.x + offset_2, cell.y + offset_2));

    data->AddBufferData(cells, color);
}

void PenTool::HandelMove(
  EditorData* data, const Cell& from, const Cell& to, const QString& color)
{
    Vector2D delta(to.x - from.x, to.y - from.y);
    double length = delta.Length();

    if (length < 1.2) {
        HandelPress(data, to, color);
    }

    // size的关系 效率低，待优化
    delta.Normalize();
    for (int i = 0; i <= length; i++) {
        const int offset_x = static_cast<int>(delta.X() * i); // 向下取整
        const int offset_y = static_cast<int>(delta.Y() * i); // 向下取整
        const Cell cell(from.x + offset_x, from.y + offset_y);
        HandelPress(data, cell, color);
    }
}

void PenTool::HandelRelease(EditorData* data, const Cell& cellr)
{
    Q_UNUSED(cellr)
    data->SaveBuffer();
}

void PenTool::InitView()
{
    size_box_ = new QSpinBox();
    connect(size_box_, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &PenTool::HandleSpinBoxChange);
    size_box_->setFocusPolicy(Qt::NoFocus);

    size_box_->setValue(1);
    size_box_->setRange(1, 10);

    attribute_wids_ << size_box_;
}

void PenTool::HandleSpinBoxChange(int data)
{
    auto* box = qobject_cast<QSpinBox*>(sender());
    if (box == size_box_) {
        size_ = data;
    }
}
