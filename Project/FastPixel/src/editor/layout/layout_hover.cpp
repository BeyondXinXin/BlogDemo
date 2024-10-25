#include "layout_hover.h"

#include "editor_data.h"
#include "qpixmap.h"

#include <QPainter>
#include <QPixmap>

LayoutHover::LayoutHover(QGraphicsItem* parent)
  : Layout(parent)
  , hover_brush_(QBrush(QColor(255, 255, 255, 100)))
{
    setAcceptHoverEvents(false);
    hovered_ = false;
}

void LayoutHover::paint(
  QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(painter)
    Q_UNUSED(widget)

    if (!hovered_) {
        return;
    }

    if (hover_enable_) {

        painter->setPen(Qt::NoPen);
        painter->setBrush(hover_brush_);
        painter->drawRect(hover_rect_);

        //        double start = clock();
        //        for (int i = 0; i < 100; i++) {

        //            //            GetHoverPix();
        //            //            painter->drawPixmap(0, 0, pix_);
        //        }
        //        double end = clock();
        //        qDebug() << (end - start) / 1000 << "s";
    }
}

void LayoutHover::MouseHover(const Cell& cell)
{
    hovered_ = data_->Contains(cell);
    if (hovered_) {
        hover_rect_ = data_->CellToRectF(cell);
    }
    update();
}

void LayoutHover::SetHoverEnable(const bool& show)
{
    hover_enable_ = show;
    hover_rect_ = QRectF();
    update();
}

bool LayoutHover::HoverEnable() const
{
    return hover_enable_;
}

bool LayoutHover::Hovered() const
{
    return hovered_;
}

void LayoutHover::GetHoverPix()
{
    pix_.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&pix_);
    painter.setPen(Qt::NoPen);
    painter.setBrush(hover_brush_);
    painter.drawRect(hover_rect_);
    painter.end();
}

void LayoutHover::Resize(int width, int heigh)
{
    rect_ = QRectF(0, 0, width, heigh);
    update();

    pix_ = QPixmap(width, heigh);
}
