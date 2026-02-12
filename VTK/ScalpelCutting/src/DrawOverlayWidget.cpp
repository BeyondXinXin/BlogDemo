#include "DrawOverlayWidget.h"

#include <QPainter>
#include <QPainterPath>

DrawOverlayWidget::DrawOverlayWidget(QWidget * parent)
  : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
}

void DrawOverlayWidget::SetPoints(const QList<QPointF> & points)
{
    m_Points = points;
    update();
}

void DrawOverlayWidget::paintEvent(QPaintEvent * event)
{
    if (m_Points.isEmpty()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path(m_Points[0]);
    for (int i = 1; i < m_Points.size(); ++i) {
        path.lineTo(m_Points[i]);
    }

    QPen pen(Qt::green, 2);
    painter.setPen(pen);
    painter.setBrush(QBrush(Qt::green, Qt::Dense4Pattern));
    painter.drawPath(path);
    painter.drawLine(m_Points[0], m_Points.last());
}