#include "layout.h"

#include "editor/editor_data.h"

Layout::Layout(QGraphicsItem* parent)
  : QGraphicsItem(parent)
{
}

Layout::~Layout()
{
}

QRectF Layout::boundingRect() const
{
    return rect_;
}

void Layout::Resize(int width, int heigh)
{
    rect_ = QRectF(0, 0, width, heigh);
    update();
}

void Layout::SetData(EditorData* data)
{
    data_ = data;
}
