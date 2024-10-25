#include "editor_data.h"

#include <QBrush>
#include <QPainter>
#include <QPixmap>
#include <QVector2D>

BrushMap EditorData::colour_bruch_ = {};

EditorData::EditorData(int num_cols, int num_rows, int cell_size)
  : num_cols_(num_cols)
  , num_rows_(num_rows)
  , cell_size_(cell_size)
{
    colour_bruch_.reserve(256);
    color_cells_.reserve(256);

    buffer_cell_.reserve(num_cols_ * num_rows_ + 5);
    colors_.reserve(num_cols_ * num_rows_ + 5);

    for (int i = 0; i < num_cols_ * num_rows_; i++) {
        colors_ << "transparent";
    }

    //    AppendStack();
}

// 所有像素块节点
QList<Cell> EditorData::Cells() const
{
    return Cells(Cell(0, 0), Cell(num_cols_, num_rows_));
}

// 由左上角节点和右下角节点定义的矩形区域包围的节点
QList<Cell> EditorData::Cells(const Cell& top_left,
                              const Cell& bottom_right) const
{
    QList<Cell> cells;
    for (int x = top_left.x, max_x = bottom_right.x; x <= max_x; x++) {
        for (int y = top_left.y, max_y = bottom_right.y; y <= max_y; y++) {
            if (Contains(Cell(x, y))) {
                cells << Cell(x, y);
            }
        }
    }
    return cells;
}

// 像素快是否再图内
bool EditorData::Contains(const Cell& node) const
{
    return (node.x >= 0 && node.y >= 0 && node.x < num_cols_ && node.y < num_rows_);
}

Cell EditorData::PointToCell(const QPoint& pos) const
{
    int cellX = static_cast<int>(pos.x() / cell_size_);
    int cellY = static_cast<int>(pos.y() / cell_size_);
    return Cell(cellX, cellY);
}

Cell EditorData::PointToCell(const QPointF& pos) const
{
    int cellX = static_cast<int>(pos.x() / cell_size_);
    int cellY = static_cast<int>(pos.y() / cell_size_);
    return Cell(cellX, cellY);
}

QPoint EditorData::CellToPoint(const Cell& cell)
{
    return QPoint(cell.x * cell_size_ + cell_size_ / 2,
                  cell.y * cell_size_ + cell_size_ / 2);
}

QRect EditorData::CellToRect(const Cell& cell) const
{
    return QRect(cell.x * cell_size_, cell.y * cell_size_, cell_size_,
                 cell_size_);
}

QRectF EditorData::CellToRectF(const Cell& cell) const
{
    return QRectF(cell.x * cell_size_, cell.y * cell_size_, cell_size_,
                  cell_size_);
}

// 获取当前层 颜色对应像素
ColorRects EditorData::GetColourRects(const QString& name) const
{
    QVector<QRect> rects;
    if (name != "transparent") {
        foreach (auto var, color_cells_.value(name)) {
            rects << QRect(var.x * cell_size_, var.y * cell_size_, cell_size_,
                           cell_size_);
        }
    }
    return rects;
}

// 获取缓存数据
CellColor EditorData::GetBufferData() const
{
    return buffer_cell_;
}

// 修改缓存数据
void EditorData::ChangeBufferCell(
  const Cell& cell, const QString& color)
{
    buffer_cell_[cell] = color;
}

// 增加缓存数据
void EditorData::AddBufferData(
  const QList<Cell>& cells, const QString& color)
{
    foreach (auto cell, cells) {
        buffer_cell_[cell] = color;
    }
}

// 设置缓存数据
void EditorData::SetBufferData(const CellColor& data)
{
    buffer_cell_.clear();
    buffer_cell_ = data;
}

// 缓存数据改为真实数据
void EditorData::SaveBuffer()
{
    AppendStack();

    auto itea = buffer_cell_.begin();
    while (itea != buffer_cell_.end()) {
        ChangePixel(itea.key(), itea.value());
        itea++;
    }

    buffer_cell_.clear();
}

void EditorData::SaveBufferNoAppendStack()
{
    auto itea = buffer_cell_.begin();
    while (itea != buffer_cell_.end()) {
        ChangePixel(itea.key(), itea.value());
        itea++;
    }

    buffer_cell_.clear();
}

// 修改 cell 颜色
void EditorData::ChangePixel(const Cell& cell, const QString& color)
{
    if (!Contains(cell)) {
        return;
    }

    auto last_color = colors_.at(cell.x * num_cols_ + cell.y);
    colors_[cell.x * num_cols_ + cell.y] = color;
    color_cells_[last_color].removeOne(cell);
    color_cells_[color] << cell;
}

// 当前数据转为图片
QPixmap EditorData::GetCurrentPix() const
{
    QPixmap pix = QPixmap(Width(), Height());
    pix.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&pix);
    painter.setPen(Qt::NoPen);
    foreach (auto colour, color_cells_.keys()) {
        painter.setBrush(GetBrush(colour));
        painter.drawRects(GetColourRects(colour));
    }
    painter.end();

    return pix;
}

void EditorData::Redo()
{
    if (redostack_color_cells_.size()) {

        undostack_color_cells_.push(color_cells_);
        undostack_colors_.push(colors_);

        color_cells_ = redostack_color_cells_.pop();
        colors_ = redostack_colors_.pop();
    }
}

void EditorData::Undo()
{
    //    qDebug() << undostack_color_cells_.size();
    if (undostack_color_cells_.size()) {

        redostack_color_cells_.push(color_cells_);
        redostack_colors_.push(colors_);

        color_cells_ = undostack_color_cells_.pop();
        colors_ = undostack_colors_.pop();
    }
}

void EditorData::AppendStack()
{
    undostack_color_cells_.push(color_cells_);
    undostack_colors_.push(colors_);

    redostack_color_cells_.clear();
    redostack_colors_.clear();
}

// 获取像素刷
QBrush EditorData::GetBrush(QString& color_name)
{
    if (!colour_bruch_.contains(color_name)) {
        colour_bruch_[color_name] = QBrush(QColor(color_name));
    }
    return colour_bruch_[color_name];
}
