#include "ellipse_tool.h"

#include "data/vector2d.h"
#include "editor/edit_scene.h"
#include "editor/edit_view.h"
#include "editor/editor_data.h"

#include <QtMath>

EllipseTool::EllipseTool()
  : Tool()
{
    setToolTip(tr("Ellipse"));
    setIcon(QIcon(QPixmap(":/png/ellipse.png")));

    cursor_ = QCursor(QPixmap(":/png/ellipse.png"), -16, -16);
}

void EllipseTool::HandelPress(EditorData* data, const Cell& cell, const QString& color)
{
    start_cell_ = cell;
    end_cell_ = cell;
    CalculationBufferData(color);

    data->SetBufferData(buffer_data_);
}

void EllipseTool::HandelMove(
  EditorData* data, const Cell& from, const Cell& to, const QString& color)
{
    end_cell_ = to;
    CalculationBufferData(color);

    data->SetBufferData(buffer_data_);
}

void EllipseTool::HandelRelease(EditorData* data, const Cell& cellr)
{
    Q_UNUSED(cellr)

    buffer_data_.remove(start_cell_);
    buffer_data_.remove(end_cell_);
    data->SetBufferData(buffer_data_);
    data->SaveBuffer();
    buffer_data_.clear();
}

void EllipseTool::CalculationBufferData(const QString& color)
{
    buffer_data_.clear();

    if (abs(start_cell_.x - end_cell_.x) < 4
        || abs(start_cell_.y - end_cell_.y) < 4) {

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

    lf_cell_ = start_cell_;
    rd_cell_ = end_cell_;
    ComparePoint(lf_cell_, rd_cell_);

    const double A = (rd_cell_.x - lf_cell_.x) / 2.0;
    const double B = (rd_cell_.y - lf_cell_.y) / 2.0;
    const double c_x = lf_cell_.x + A;
    const double c_y = lf_cell_.y + B;

    const int mince = A * B * 10;
    const double offset_angle = 360.0 / mince;

    for (double a = 0; (a - 360.0) < 1e-4; a += offset_angle) {
        const double r = qDegreesToRadians(a);
        const double R = A * B / sqrt(pow(A * sin(r), 2) + pow(B * cos(r), 2)); //计算对应角度的半径
        const int x = round(c_x + R * cos(r));
        const int y = round(c_y + R * sin(r));

        buffer_data_[Cell(x, y)] = color;
    }

    buffer_data_[start_cell_] = color;
    buffer_data_[end_cell_] = color;
}

void EllipseTool::ComparePoint(Cell& lt, Cell& rd)
{
    Cell l = lt;
    Cell r = rd;

    if (l.x <= r.x) {
        if (l.y <= r.y) {
        } else {
            lt.y = r.y;
            rd.y = l.y;
        }
    } else {
        if (l.y < r.y) {
            lt.x = r.x;
            rd.x = l.x;
        } else {
            Cell tmp;
            tmp = lt;
            lt = rd;
            rd = tmp;
        }
    }
}
