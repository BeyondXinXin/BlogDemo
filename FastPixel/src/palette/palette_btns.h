#ifndef PALETTE_BTNS_H
#define PALETTE_BTNS_H

#include <QPen>

#include "palete_widget.h"

class PaletteBtns : public PaleteWidget
{
    Q_OBJECT
public:
    PaletteBtns(PaletteManager* manager = nullptr, QWidget* parent = nullptr);
    ~PaletteBtns() override;

    void UpdataBgPix() override;
    void UpdataCurColor(ColorType c_type) override;

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

    void DrawDecorate(QPainter* painter) override;

private:
    int BtnIdByPos(const QPoint& pos);
    QPolygon GetDecorateTriangle(ColorType c_type, const QRect rect) const;

private:
    QColor bg_color_;
    QSize size_;
    int btns_col_;
    int btns_row_;
    int space_;

    int hover_id_;

    QBrush decorate_brush_;
    QPen decorate_pen_;

    QList<QRect> btn_rects_;
};

#endif // PALETTE_BTNS_H
