#include "palette_btns.h"

#include "palette_manager.h"

#include <QPainter>
#include <QMouseEvent>

PaletteBtns::PaletteBtns(PaletteManager* manager, QWidget* parent)
  : PaleteWidget(manager, parent)
  , bg_color_(QColor(160, 160, 160))
  , size_(40, 40)
  , btns_col_(0)
  , btns_row_(0)
  , space_(5)
  , hover_id_(-1)
{
    decorate_brush_ = QBrush(Qt::black);
    decorate_pen_ = QPen(QColor(225, 215, 0), 2);
}

PaletteBtns::~PaletteBtns()
{
}

/**
 * @brief PaletteBtns::UpdataBgPix
 * 更新背景图片
 */
void PaletteBtns::UpdataBgPix()
{
    btn_rects_.clear();

    auto colors = Manager()->GetColors();

    const int w = width();
    const int h = height();
    const int num = colors.size();

    btns_col_ = static_cast<int>(
      floor((w - space_ * 2) / (size_.width() + space_)));
    btns_row_ = 1 + num / btns_col_;
    bg_pix_ = QPixmap(w, h);
    bg_pix_.fill(bg_color_);

    QPainter painter;
    painter.begin(&bg_pix_);

    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setPen(Qt::NoPen);

    for (int y = 0; y < btns_row_; y++) {
        for (int x = 0; x < btns_col_; x++) {
            int id = y * btns_col_ + x;
            if (id >= num) {
                break;
            }

            QBrush brush(colors.at(id));
            painter.setBrush(brush);
            QRect rect(space_ + x * (size_.width() + space_),
                       space_ + y * (size_.height() + space_),
                       size_.width(),
                       size_.height());
            btn_rects_ << rect;
            painter.drawRect(rect);
        }
    }

    painter.end();
    update();
}

void PaletteBtns::UpdataCurColor(ColorType c_type)
{
    Q_UNUSED(c_type)
    UpdataBgPix();
    update();
}

void PaletteBtns::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        int id = BtnIdByPos(e->pos());
        if (id >= 0 && manager_->GetColorId(K_CT_Primary) != id) {
            emit SgnSelectColor(K_CT_Primary, id);
            update();
        }
    } else if (e->button() == Qt::RightButton) {
        int id = BtnIdByPos(e->pos());
        if (id >= 0 && manager_->GetColorId(K_CT_Secondary) != id) {
            emit SgnSelectColor(K_CT_Secondary, id);
            update();
        }
    }
}

void PaletteBtns::mouseMoveEvent(QMouseEvent* e)
{
    int hover_id = BtnIdByPos(e->pos());
    if (hover_id_ != hover_id) {
        hover_id_ = hover_id;
        update();
    }
}

void PaletteBtns::mouseReleaseEvent(QMouseEvent* e)
{
    QWidget::mouseReleaseEvent(e);
}

void PaletteBtns::DrawDecorate(QPainter* painter)
{
    painter->save();

    int btn_size = Manager()->GetColorsSize();

    if (hover_id_ >= 0 && hover_id_ < btn_size) {
        auto& rect = btn_rects_.at(hover_id_);
        QPen pen(Qt::white, 4);
        painter->setPen(pen);
        painter->drawRect(rect);
    }

    const int primary_id = manager_->GetColorId(K_CT_Primary);
    if (primary_id >= 0 && primary_id < btn_rects_.size()) {
        painter->setPen(decorate_pen_);
        painter->setBrush(decorate_brush_);
        painter->drawPolygon(GetDecorateTriangle(K_CT_Primary, btn_rects_.at(primary_id)));
    }

    const int secondary_id = manager_->GetColorId(K_CT_Secondary);
    if (secondary_id >= 0 && secondary_id < btn_rects_.size()) {
        painter->setPen(decorate_pen_);
        painter->setBrush(decorate_brush_);
        painter->drawPolygon(GetDecorateTriangle(K_CT_Secondary, btn_rects_.at(secondary_id)));
    }

    painter->restore();
}

QPolygon PaletteBtns::GetDecorateTriangle(
  ColorType c_type, const QRect rect) const
{
    QPolygon triangle(3);

    switch (c_type) {
    case K_CT_Primary: {
        triangle.setPoint(0, rect.topLeft());
        triangle.setPoint(1, rect.topLeft() + QPoint(20, 0));
        triangle.setPoint(2, rect.topLeft() + QPoint(0, 20));
    } break;
    case K_CT_Secondary: {
        triangle.setPoint(0, rect.bottomRight());
        triangle.setPoint(1, rect.bottomRight() + QPoint(-20, 0));
        triangle.setPoint(2, rect.bottomRight() + QPoint(0, -20));
    } break;
    default: {

    } break;
    }

    return triangle;
}

int PaletteBtns::BtnIdByPos(const QPoint& pos)
{
    // 效率底，先实现功能
    for (int i = 0; i < btn_rects_.size(); i++) {
        if (btn_rects_.at(i).contains(pos)) {
            return i;
        }
    }
    return -1;
}
