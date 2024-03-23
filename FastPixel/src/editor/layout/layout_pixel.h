#ifndef LAYOUTPIXEL_H
#define LAYOUTPIXEL_H

#include <QGraphicsItem>

class EditorData;

class LayoutPixel : public QGraphicsItem
{
public:
    LayoutPixel(QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* item, QWidget* widget) override;
    void Resize(int width, int heigh);

    void SetData(EditorData* data);

    void UpdataBg();

private:
    QRectF rect_;
    double default_length_;

    EditorData* data_;

    QPixmap pix_;
};

#endif // LAYOUTPIXEL_H
