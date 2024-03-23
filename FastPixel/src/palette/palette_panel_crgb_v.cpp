#include "palette_panel_crgb_v.h"

#include "palette_manager.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

#define PI 3.1415936

PalettePanel_CRGB_V::PalettePanel_CRGB_V(PaletteManager* manager, QWidget* parent)
  : PaleteWidget(manager, parent)
  , bar_height_(40)
  , cur_color_(Qt::red)
  , pos_r_(8)
{
    pos_pen_.setColor(Qt::white);
    pos_pen_.setWidth(3);

    cirale_edge_ = 0;
}

PalettePanel_CRGB_V::~PalettePanel_CRGB_V()
{
}

void PalettePanel_CRGB_V::UpdataBgPix()
{
    const int w = width();
    const int h = height();
    const int panel_h = height() - bar_height_;

    bg_pix_ = QPixmap(w, h);
    bg_pix_.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&bg_pix_);

    // 画 panel

    const double r = w > panel_h ? panel_h / 2.0 - 5 : w / 2.0 - 5;
    QPointF pos_center;
    if (w > panel_h) {
        pos_center = QPointF(w / 2.0, cirale_edge_ / 2 + panel_h / 2.0);
    } else {
        pos_center = QPointF(cirale_edge_ / 2 + w / 2.0, panel_h / 2.0);
    }

    QColor start, center, end;
    double hue = cur_color_.hsvHueF();
    double value = cur_color_.valueF();

    for (double i = 0.0; i < 360.0; i = i + .5) {

        QLinearGradient linear;
        double x = pos_center.x() + r * cos(i * PI / 180);
        double y = pos_center.y() + r * sin(i * PI / 180);
        QPointF pos_end(x, y);

        double hue = 1.0 - i / static_cast<double>(360);

        start.setHsvF(hue, 0, value);
        center.setHsvF(hue, 0.5, value);
        end.setHsvF(hue, 1, value);

        linear.setStart(pos_center);
        linear.setFinalStop(pos_end);

        linear.setColorAt(0.0, start);
        linear.setColorAt(0.5, center);
        linear.setColorAt(1.0, end);

        painter.setPen(QPen(linear, 5));
        painter.drawLine(pos_center, pos_end);
    }

    // 画 bar
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

void PalettePanel_CRGB_V::UpdataCurColor(ColorType c_type)
{
    if (this->isVisible()) {
        cur_color_ = Manager()->GetColor(c_type);
        UpdataBgPix();
        update();
    }
}

void PalettePanel_CRGB_V::mousePressEvent(QMouseEvent* e)
{
    PaleteWidget::mousePressEvent(e);
}

void PalettePanel_CRGB_V::mouseMoveEvent(QMouseEvent* e)
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

void PalettePanel_CRGB_V::mouseReleaseEvent(QMouseEvent* e)
{
    PaleteWidget::mouseReleaseEvent(e);
}

void PalettePanel_CRGB_V::DrawDecorate(QPainter* painter)
{
    painter->save();

    const int w = width() - cirale_edge_;
    const int h = height() - bar_height_ - cirale_edge_;
    const double r = w > h ? h / 2.0 - 5 : w / 2.0 - 5;
    const double pos_r = r * cur_color_.hsvSaturationF();
    const double alpha = (1 - cur_color_.hsvHueF()) * 360;

    QPointF pos_center;
    if (w > h) {
        pos_center = QPointF(w / 2.0, cirale_edge_ / 2 + h / 2.0);
    } else {
        pos_center = QPointF(cirale_edge_ / 2 + w / 2.0, h / 2.0);
    }

    double x = pos_center.x() + pos_r * cos(alpha * PI / 180);
    double y = pos_center.y() + pos_r * sin(alpha * PI / 180);

    painter->setPen(pos_pen_);
    painter->drawEllipse(QPointF(x, y), pos_r_, pos_r_);

    x = w * cur_color_.valueF();
    y = height() - bar_height_ / 2.0;

    painter->setPen(pos_pen_);
    painter->drawEllipse(QPointF(x, y), pos_r_, pos_r_);

    painter->restore();
}

void PalettePanel_CRGB_V::HandlePanelMove(int x, int y)
{
    const double w = width();
    const double h = height() - bar_height_;
    const double r = w > h ? h / 2.0 - 5 : w / 2.0 - 5;
    QPointF pos_center;

    if (w > h) {
        pos_center = QPointF(w / 2.0, cirale_edge_ / 2 + h / 2.0);
    } else {
        pos_center = QPointF(cirale_edge_ / 2 + w / 2.0, h / 2.0);
    }

    double new_s = (x - pos_center.x()) * (x - pos_center.x())
      + (y - pos_center.y()) * (y - pos_center.y());
    new_s = pow(new_s, .5);
    new_s = new_s / r;
    new_s = new_s > 1 ? 1 : new_s;

    double new_h = atan2(-(y - pos_center.y()), (x - pos_center.x())) * 180.0 / PI;

    new_h = new_h < 0 ? 360 + new_h : new_h;
    new_h = new_h / 360.0;

    cur_color_.setHsvF(
      new_h,
      new_s,
      cur_color_.valueF());
}

void PalettePanel_CRGB_V::HandleBarMove(int x, int y)
{
    Q_UNUSED(y)

    const double new_v = x / static_cast<double>(width());
    cur_color_.setHsvF(
      cur_color_.hsvHueF(),
      cur_color_.hsvSaturationF(),
      new_v);
}
