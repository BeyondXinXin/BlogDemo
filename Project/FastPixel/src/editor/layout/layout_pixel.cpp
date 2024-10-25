#include "layout_pixel.h"

#include "editor_data.h"

#include <QPainter>

LayoutPixel::LayoutPixel(QGraphicsItem* parent)
  : QGraphicsItem(parent)
  , rect_(QRect(0, 0, 32 * 8, 32 * 8))
  , default_length_(32)
  , data_(nullptr)
{
}

QRectF LayoutPixel::boundingRect() const
{
    return rect_;
}

void LayoutPixel::paint(
  QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    //    painter->drawPixmap(0, 0, pix_);

    painter->setPen(Qt::NoPen);
    auto colors = data_->GetLayourColours();
    foreach (auto colour, colors) {
        painter->setBrush(data_->GetBrush(colour));
        painter->drawRects(data_->GetColourRects(colour));
    }

    auto buffers = data_->GetBufferData();
    auto itea = buffers.begin();
    while (itea != buffers.end()) {
        painter->setBrush(data_->GetBrush(itea.value()));
        painter->drawRect(data_->CellToRect(itea.key()));
        itea++;
    }
}

void LayoutPixel::Resize(int width, int heigh)
{
    rect_ = QRectF(0, 0, width, heigh);
    update();

    pix_ = QPixmap(rect_.width(), rect_.height());
    pix_.fill(Qt::transparent);
}

void LayoutPixel::SetData(EditorData* data)
{
    data_ = data;
}

void LayoutPixel::UpdataBg()
{
    pix_ = data_->GetCurrentPix();
}
