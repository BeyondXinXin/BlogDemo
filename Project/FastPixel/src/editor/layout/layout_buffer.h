#ifndef LAYOUTBUFFER_H
#define LAYOUTBUFFER_H

#include "data/cell.h"
#include "editor/layout/layout.h"

#include <QBrush>
#include <QPen>

class SelectAreaItem;

class LayoutBuffer : public Layout, public QObject
{
public:
    LayoutBuffer(QGraphicsItem* parent = nullptr);
    ~LayoutBuffer() override;

    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* item,
               QWidget* widget) override;

    void SetFirstCell(const Cell& cell);
    void SetEndCell(const Cell& cell);

    void MoveCell(const Cell& offset);
    void End();

    void HandelKeyEvent(QKeyEvent* event);

    bool Contain(const Cell& cell) const;

    void StartMove();

    void ClearBuffer();

private:
    void UpdateAntValue();

private:
    Cell lf_cell_;
    Cell rd_cell_;
    Cell first_cell_;
    Cell second_cell_;

    QHash<Cell, QString> buffer_data_;

    bool show_;

    QPen select_pen_;
    int dashes_;
    int spaces_;
    int line_len_;
    int line_step_;
    int line_speed_;
    QVector<double> dash_pattern_; //线条样式数据

    QTimer* timer_;
};

#endif // LAYOUTBUFFER_H
