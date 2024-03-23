#include "palete_widget.h"

#include "palette_manager.h"

#include <QPainter>

PaleteWidget::PaleteWidget(
  PaletteManager* manager, QWidget* parent)
  : QWidget(parent)
  , manager_(manager)
{
    setMouseTracking(true);

    setWindowFlags(Qt::WindowCloseButtonHint);
}

PaleteWidget::~PaleteWidget()
{
}

void PaleteWidget::SetManager(PaletteManager* manager)
{
    manager_ = manager;
}

void PaleteWidget::UpdataCurColor(ColorType c_type)
{
    Q_UNUSED(c_type)
    update();
}

void PaleteWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    DrawBg(&painter);
    DrawDecorate(&painter);
}

void PaleteWidget::resizeEvent(QResizeEvent* event)
{
    if (this->isVisible()) {
        UpdataBgPix();
        QWidget::resizeEvent(event);
    }
}

void PaleteWidget::DrawBg(QPainter* painter)
{
    painter->save();
    painter->drawPixmap(0, 0, bg_pix_);
    painter->restore();
}

PaletteManager* PaleteWidget::Manager()
{
    return manager_;
}
