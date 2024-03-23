#ifndef PALETTEPANEL_CRGB_V_H
#define PALETTEPANEL_CRGB_V_H

#include "palete_widget.h"

#include <QPen>

class PalettePanel_CRGB_V : public PaleteWidget
{
    Q_OBJECT
public:
    PalettePanel_CRGB_V(PaletteManager* manager = nullptr, QWidget* parent = nullptr);
    ~PalettePanel_CRGB_V() override;

    void UpdataBgPix() override;
    void UpdataCurColor(ColorType c_type) override;

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

    void DrawDecorate(QPainter* painter) override;

private:
    void HandlePanelMove(int x, int y);
    void HandleBarMove(int x, int y);

private:
    int bar_height_;
    QColor cur_color_;

    int pos_r_;
    QPen pos_pen_;

    int cirale_edge_;
};

#endif // PALETTEPANEL_CRGB_V_H
