#ifndef PENTOOL_H
#define PENTOOL_H

#include "tool.h"

class QSpinBox;

// 笔
class PenTool : public Tool
{
    Q_OBJECT
public:
    PenTool();

    void HandelPress(EditorData* data, const Cell& cell,
                     const QString& color) override;
    void HandelMove(EditorData* data, const Cell& from,
                    const Cell& to, const QString& color) override;
    void HandelRelease(EditorData* data, const Cell& cellr) override;

private:
    void InitView();

    void HandleSpinBoxChange(int data);

private:
    QList<Cell> buffer_data_;

    QSpinBox* size_box_;

    int size_;
};

#endif // PENTOOL_H
