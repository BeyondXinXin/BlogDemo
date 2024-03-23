#include "palette_panel_hs_v.h"

#include "palette_manager.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

PalettePanel_HS_V::PalettePanel_HS_V(PaletteManager* manager, QWidget* parent)
  : PaleteWidget(manager, parent)
  , bar_height_(40)
  , cur_color_(Qt::red)
  , pos_r_(8)
{
    pos_pen_.setColor(Qt::white);
    pos_pen_.setWidth(3);
}

PalettePanel_HS_V::~PalettePanel_HS_V()
{
}

void PalettePanel_HS_V::UpdataBgPix()
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
    double value = cur_color_.valueF();
    for (int i = 0; i < w; i++) {
        QLinearGradient linear;
        linear.setStart(QPointF(i, 0));
        linear.setFinalStop(QPointF(i, panel_h));
        double rote = (i) / double(w);
        start.setHsvF(rote, 1.0, value);
        center.setHsvF(rote, .5, value);
        end.setHsvF(rote, 0.0, value);

        linear.setColorAt(0.0, start);
        linear.setColorAt(0.5, center);
        linear.setColorAt(1.0, end);

        painter.setPen(QPen(linear, 1));
        painter.drawLine(QPointF(i, 0), QPointF(i, panel_h));
    }

    // 画 bar
    double hue = cur_color_.hsvHueF();
    double saturation = cur_color_.saturationF();
    QLinearGradient linear;
    linear.setStart(QPointF(0, 0));
    linear.setFinalStop(QPointF(w, h));

    start.setHsvF(hue, saturation, 0);
    center.setHsvF(hue, saturation, 0.5);
    end.setHsvF(hue, saturation, 1);

    linear.setColorAt(0.0, start);
    linear.setColorAt(0.5, center);
    linear.setColorAt(1.0, end);

    painter.setPen(Qt::NoPen);
    painter.setBrush(linear);
    painter.drawRect(0, panel_h, w, bar_height_);

    painter.end();
}

void PalettePanel_HS_V::UpdataCurColor(ColorType c_type)
{
    if (this->isVisible()) {
        cur_color_ = Manager()->GetColor(c_type);
        UpdataBgPix();
        update();
    }
}

void PalettePanel_HS_V::mousePressEvent(QMouseEvent* e)
{
    PaleteWidget::mousePressEvent(e);
}

void PalettePanel_HS_V::mouseMoveEvent(QMouseEvent* e)
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

void PalettePanel_HS_V::mouseReleaseEvent(QMouseEvent* e)
{
    PaleteWidget::mouseReleaseEvent(e);
}

void PalettePanel_HS_V::DrawDecorate(QPainter* painter)
{
    painter->save();

    const double w = static_cast<double>(width());
    const double h = static_cast<double>(height()) - bar_height_;
    double x = w * cur_color_.hsvHueF();
    double y = h * (1 - cur_color_.hsvSaturationF());

    painter->setPen(pos_pen_);
    painter->drawEllipse(QPointF(x, y), pos_r_, pos_r_);

    x = w * cur_color_.valueF();
    y = height() - bar_height_ / 2.0;

    painter->setPen(pos_pen_);
    painter->drawEllipse(QPointF(x, y), pos_r_, pos_r_);

    painter->restore();
}

void PalettePanel_HS_V::HandlePanelMove(int x, int y)
{
    const double new_h = x / static_cast<double>(width());
    const double new_s = 1.0 - y / static_cast<double>(height() - bar_height_);
    cur_color_.setHsvF(
      new_h,
      new_s,
      cur_color_.valueF());
}

void PalettePanel_HS_V::HandleBarMove(int x, int y)
{
    Q_UNUSED(y)

    const double new_v = x / static_cast<double>(width());
    cur_color_.setHsvF(
      cur_color_.hsvHueF(),
      cur_color_.hsvSaturationF(),
      new_v);
}
