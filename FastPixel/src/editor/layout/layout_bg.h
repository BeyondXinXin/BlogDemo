#ifndef IMGLAYOUTTRANSPARENTBG_H
#define IMGLAYOUTTRANSPARENTBG_H

#include "layout.h"
#include <QBrush>

class LayoutTransparentBG : public Layout
{
public:
    LayoutTransparentBG(QGraphicsItem* parent = nullptr);

    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* item,
               QWidget* widget) override;

    void Resize(int width, int heigh) override;

public:
    QBrush bg_color1_;
    QBrush bg_color2_;
    double default_length_;
    QVector<QRectF> rects_;
};

#endif // IMGLAYOUTTRANSPARENTBG_H
