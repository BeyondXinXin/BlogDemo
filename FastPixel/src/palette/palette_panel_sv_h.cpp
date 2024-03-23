#include "palette_panel_sv_h.h"

#include "palette_manager.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

PalettePanel_SV_H::PalettePanel_SV_H(PaletteManager* manager, QWidget* parent)
  : PaleteWidget(manager, parent)
  , bar_height_(40)
  , cur_color_(Qt::red)
  , pos_r_(8)
{
    pos_pen_.setColor(Qt::white);
    pos_pen_.setWidth(3);
}

PalettePanel_SV_H::~PalettePanel_SV_H()
{
}

void PalettePanel_SV_H::UpdataBgPix()
{
    const int w = width();
    const int h = height();
    const int panel_h = height() - bar_height_;

    bg_pix_ = QPixmap(w, h);
    bg_pix_.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&bg_pix_);

    // 画 panel
    QColor start, center, end;
    double hue = cur_color_.hsvHueF();
    for (int i = 0; i < w; i++) {
        QLinearGradient linear;
        linear.setStart(QPointF(i, 0));
        linear.setFinalStop(QPointF(i, h));

        double rote = i / double(w);
        start.setHsvF(hue, rote, 1.0);
        center.setHsvF(hue, rote, 0.5);
        end.setHsvF(hue, rote, 0.0);

        linear.setColorAt(0.0, start);
        linear.setColorAt(0.5, center);
        linear.setColorAt(1.0, end);

        painter.setPen(QPen(linear, 1));
        painter.drawLine(QPointF(i, 0), QPointF(i, h));
    }

    // 画 bar
    QLinearGradient linear;
    linear.setStart(QPointF(0, panel_h));
    linear.setFinalStop(QPointF(w, h));

    double step = 1.0 / 16.0;
    for (double i = 0.0; (1.0 - i) >= .0e6; i += step) {
        linear.setColorAt(i, QColor::fromHsvF(i, 1, 1));
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(linear);
    painter.drawRect(0, panel_h, w, bar_height_);

    painter.end();
}

void PalettePanel_SV_H::UpdataCurColor(ColorType c_type)
{
    if (this->isVisible()) {
        cur_color_ = Manager()->GetColor(c_type);
        UpdataBgPix();
        update();
    }
}

void PalettePanel_SV_H::mousePressEvent(QMouseEvent* e)
{
    PaleteWidget::mousePressEvent(e);
}

void PalettePanel_SV_H::mouseMoveEvent(QMouseEvent* e)
{
    auto mouse_buttons = QApplication::mouseButtons();
    int mouse;
    if (mouse_buttons & Qt::LeftButton) {
        mouse = 1;
    } else if (mouse_buttons & Qt::RightButton) {
        mouse = 2;
    } else {
        mouse = 0;
    }

    if (!mouse) {
        return;
    }

    int x = e->pos().x();
    int y = e->pos().y();
    x = x < 0 ? 0 : x;
    y = y < 0 ? 0 : y;
    x = (x >= width()) ? width() - 1 : x;
    y = (y >= height()) ? height() - 1 : y;

    if (y > (height() - bar_height_)) {
        HandleBarMove(x, y);
    } else {
        HandlePanelMove(x, y);
    }

    if (1 == mouse) {
        emit SgnChangeColor(K_CT_Primary, cur_color_);
    } else if (2 == mouse) {
        emit SgnChangeColor(K_CT_Secondary, cur_color_);
    }
}

void PalettePanel_SV_H::mouseReleaseEvent(QMouseEvent* e)
{
    PaleteWidget::mouseReleaseEvent(e);
}

void PalettePanel_SV_H::DrawDecorate(QPainter* painter)
{
    painter->save();

    const double w = static_cast<double>(width());
    const double h = static_cast<double>(height()) - bar_height_;
    double x = w * cur_color_.hsvSaturationF();
    double y = h * (1.0 - cur_color_.valueF());

    painter->setPen(pos_pen_);
    painter->drawEllipse(QPointF(x, y), pos_r_, pos_r_);

    x = w * cur_color_.hsvHueF();
    y = height() - bar_height_ / 2.0;

    painter->setPen(pos_pen_);
    painter->drawEllipse(QPointF(x, y), pos_r_, pos_r_);

    painter->restore();
}

void PalettePanel_SV_H::HandlePanelMove(int x, int y)
{
    const double new_s = x / static_cast<double>(width());
    const double new_v = 1.0 - y / static_cast<double>(height() - bar_height_);
    cur_color_.setHsvF(
      cur_color_.hsvHueF(),
      new_s,
      new_v);
}

void PalettePanel_SV_H::HandleBarMove(int x, int y)
{
    Q_UNUSED(y)

    const double new_h = x / static_cast<double>(width());
    cur_color_.setHsvF(
      new_h,
      cur_color_.hsvSaturationF(),
      cur_color_.valueF());
}
