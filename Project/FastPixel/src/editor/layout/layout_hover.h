#ifndef LAYOUTHOVER_H
#define LAYOUTHOVER_H

#include "data/cell.h"
#include "editor/layout/layout.h"

#include <QBrush>

class EditorData;

// 鼠标hover高亮层
class LayoutHover : public Layout
{
public:
    LayoutHover(QGraphicsItem* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* item, QWidget* widget) override;

    void MouseHover(const Cell& cell);

    void SetHoverEnable(const bool& show);
    bool HoverEnable() const;
    bool Hovered() const;

    void GetHoverPix();

    void Resize(int width, int heigh) override;

public:
    QBrush hover_brush_;
    QRectF hover_rect_;
    bool hovered_;
    bool hover_enable_;

    QPixmap pix_;
};

#endif // LAYOUTHOVER_H
