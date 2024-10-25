#ifndef PALETTEPANEL_SV_H_H
#define PALETTEPANEL_SV_H_H

#include "palete_widget.h"

#include <QPen>

class PalettePanel_SV_H : public PaleteWidget
{
    Q_OBJECT
public:
    PalettePanel_SV_H(PaletteManager* manager = nullptr, QWidget* parent = nullptr);
    ~PalettePanel_SV_H() override;

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
};

#endif // PALETTEPANEL_SV_H_H
