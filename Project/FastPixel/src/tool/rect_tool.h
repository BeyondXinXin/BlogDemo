#ifndef RECTTOOL_H
#define RECTTOOL_H

#include "tool.h"

class QPushButton;

class RectTool : public Tool
{
    Q_OBJECT
public:
    RectTool();

    void HandelPress(EditorData* data, const Cell& cell,
                     const QString& color) override;
    void HandelMove(EditorData* data, const Cell& from,
                    const Cell& to, const QString& color) override;
    void HandelRelease(EditorData* data, const Cell& cellr) override;

private:
    void CalculationBufferData(const QString& color);

private:
    Cell left_up_, right_down_;
    QHash<Cell, QString> buffer_data_;
};

#endif // RECTTOOL_H
