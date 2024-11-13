#include "layout_bg.h"

#include <QPainter>

LayoutTransparentBG::LayoutTransparentBG(QGraphicsItem* parent)
  : Layout(parent)
  , bg_color1_(QBrush(QColor(110, 110, 110)))
  , bg_color2_(QBrush(QColor(120, 120, 120)))
{
    setAcceptHoverEvents(false);
    default_length_ = 128;
}

void LayoutTransparentBG::paint(
  QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    painter->setPen(Qt::NoPen);

    painter->setBrush(bg_color1_);
    painter->drawRect(rect_);
    painter->setBrush(bg_color2_);
    painter->drawRects(rects_);
}

void LayoutTransparentBG::Resize(int width, int heigh)
{
    rect_ = QRectF(0, 0, width, heigh);
    rects_.clear();
    for (int i = 0; i < rect_.width() / default_length_; i++) {
        for (int j = i % 2; j < rect_.height() / default_length_; j = j + 2) {
            rects_ << QRectF(i * default_length_, j * default_length_, default_length_, default_length_);
        }
    }
    update();
}
