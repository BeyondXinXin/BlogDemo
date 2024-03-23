#ifndef STROKETOOL_H
#define STROKETOOL_H

#pragma execution_character_set("utf-8")

#include "tool.h"

// 线
class StrokeTool : public Tool
{
    Q_OBJECT
public:
    StrokeTool();

    void HandelPress(EditorData *data, const Cell &cell,
                     const QString &color) override;
    void HandelMove(EditorData *data, const Cell &from,
                    const Cell &to, const QString &color) override;
    void HandelRelease(EditorData *data, const Cell &cellr) override;

private:
    void CalculationBufferData(const QString &color);

private:
    Cell start_cell_, end_cell_;
    QHash<Cell, QString> buffer_data_;
};

#endif // STROKETOOL_H
