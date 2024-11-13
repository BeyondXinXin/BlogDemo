#ifndef ELLIPSETOOL_H
#define ELLIPSETOOL_H

#include "tool.h"

class EllipseTool : public Tool
{
    Q_OBJECT
public:
    EllipseTool();

    void HandelPress(EditorData *data, const Cell &cell,
                     const QString &color) override;
    void HandelMove(EditorData *data, const Cell &from,
                    const Cell &to, const QString &color) override;
    void HandelRelease(EditorData *data, const Cell &cellr) override;

private:
    void CalculationBufferData(const QString &color);
    void ComparePoint(Cell &lt, Cell &rd);

private:
    Cell start_cell_, end_cell_;
    Cell lf_cell_, rd_cell_;
    QHash<Cell, QString> buffer_data_;
};

#endif // ELLIPSETOOL_H
