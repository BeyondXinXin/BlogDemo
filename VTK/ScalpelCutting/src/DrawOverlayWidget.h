#ifndef DrawOverlayWidget_H
#define DrawOverlayWidget_H

#include <QWidget>

class DrawOverlayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DrawOverlayWidget(QWidget * parent = nullptr);

    void SetPoints(const QList<QPointF> & points);

protected:
    void paintEvent(QPaintEvent * event) override;

private:
    QList<QPointF> m_Points;
};

#endif // DrawOverlayWidget_H