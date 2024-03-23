#include "layout_buffer.h"

#include "editor/editor_data.h"

#include <QKeyEvent>
#include <QPainter>
#include <QTimer>

LayoutBuffer::LayoutBuffer(QGraphicsItem* parent)
  : Layout(parent)

{
    setAcceptHoverEvents(false);
    select_pen_.setWidthF(1.0);
    select_pen_.setColor(Qt::darkBlue);

    first_cell_ = Cell(0, 0);
    second_cell_ = Cell(0, 0);
    lf_cell_ = Cell(0, 0);
    rd_cell_ = Cell(0, 0);

    timer_ = new QTimer();
    QObject::connect(timer_, &QTimer::timeout, this, [&]() {
        UpdateAntValue();
    });
    show_ = false;

    line_len_ = 20;
    line_step_ = 5;
    line_speed_ = 100;

    dashes_ = line_len_;
    spaces_ = line_len_;
    dash_pattern_.clear();
    for (int i = 0; i < 20; ++i) {
        dash_pattern_ << line_len_;
    }

    setAcceptHoverEvents(false);
}

LayoutBuffer::~LayoutBuffer()
{
}

void LayoutBuffer::paint(
  QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (show_) {
        painter->setPen(select_pen_);
        painter->drawRect(rect_.adjusted(.2, .2, -.2, -.2));
    }
}

void LayoutBuffer::SetFirstCell(const Cell& cell)
{
    if (!timer_->isActive()) {
        timer_->start(line_speed_);
        show_ = true;
    }

    first_cell_ = cell;
    SetEndCell(cell);
}

void LayoutBuffer::SetEndCell(const Cell& cell)
{

    // 给第二个点赋值
    second_cell_ = cell;

    // 计算 左上角和右下角
    double min_x, max_x, min_y, max_y;
    if (first_cell_.x > second_cell_.x) {
        min_x = second_cell_.x;
        max_x = first_cell_.x;
    } else {
        min_x = first_cell_.x;
        max_x = second_cell_.x;
    }
    if (first_cell_.y > second_cell_.y) {
        min_y = second_cell_.y;
        max_y = first_cell_.y;
    } else {
        min_y = first_cell_.y;
        max_y = second_cell_.y;
    }

    lf_cell_ = Cell(min_x, min_y);
    rd_cell_ = Cell(max_x, max_y);

    // 调整大小和移动到左上角
    int cell_size = data_->CellSize();
    rect_ = QRectF(0, 0,
                   (1 + rd_cell_.x - lf_cell_.x) * cell_size,
                   (1 + rd_cell_.y - lf_cell_.y) * cell_size);
    setPos(lf_cell_.x * cell_size, lf_cell_.y * cell_size);
    update();
}

// 移动
void LayoutBuffer::MoveCell(const Cell& offset)
{
    QHash<Cell, QString> buffer_data;
    auto itea = buffer_data_.begin();
    while (itea != buffer_data_.end()) {
        buffer_data[itea.key() + offset] = itea.value();
        itea++;
    }
    data_->SetBufferData(buffer_data);
    buffer_data_ = buffer_data;

    lf_cell_ = lf_cell_ + offset;
    rd_cell_ = rd_cell_ + offset;

    // 调整大小和移动到左上角
    int cell_size = data_->CellSize();
    rect_ = QRectF(0, 0,
                   (1 + rd_cell_.x - lf_cell_.x) * cell_size,
                   (1 + rd_cell_.y - lf_cell_.y) * cell_size);
    setPos(lf_cell_.x * cell_size, lf_cell_.y * cell_size);
    update();
}

void LayoutBuffer::End()
{
    show_ = false;
    update();

    first_cell_ = Cell(0, 0);
    second_cell_ = Cell(0, 0);
    lf_cell_ = Cell(0, 0);
    rd_cell_ = Cell(0, 0);

    timer_->stop();
}

void LayoutBuffer::HandelKeyEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Escape: {
        End();
    }
    default:
        break;
    }
}

// 当前aoi是否包含cell
bool LayoutBuffer::Contain(const Cell& cell) const
{
    if (cell.x >= lf_cell_.x
        && cell.x <= rd_cell_.x
        && cell.y >= lf_cell_.y
        && cell.y <= rd_cell_.y) {
        return true;
    }
    return false;
}

// 开始运动
void LayoutBuffer::StartMove()
{
    data_->AppendStack();
    QList<Cell> cells = data_->Cells(lf_cell_, rd_cell_);
    buffer_data_.clear();
    foreach (auto var, cells) {
        buffer_data_[var] = data_->GetCellColor(var);
        data_->ChangePixel(var, "transparent");
    }
    data_->SetBufferData(buffer_data_);
}

// 清空区域内数据
void LayoutBuffer::ClearBuffer()
{
    data_->AppendStack();
    QList<Cell> cells = data_->Cells(lf_cell_, rd_cell_);
    foreach (auto var, cells) {
        data_->ChangePixel(var, "transparent");
    }
    buffer_data_.clear();
    data_->SetBufferData(buffer_data_);
    End();
}

// 蚂蚁线
void LayoutBuffer::UpdateAntValue()
{
    if (dashes_ == line_len_ && spaces_ == line_len_) {
        dashes_ = 0;
        spaces_ = 0;
    }

    if (dashes_ == 0 && spaces_ < line_len_) {
        spaces_ = spaces_ + line_step_;
    } else if (spaces_ == line_len_ && dashes_ < line_len_) {
        dashes_ = dashes_ + line_step_;
    }

    dash_pattern_[0] = dashes_;
    dash_pattern_[1] = spaces_;

    select_pen_.setDashPattern(dash_pattern_);
    update();
}
