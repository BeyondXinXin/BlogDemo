#ifndef RUBBERTOOL_H
#define RUBBERTOOL_H

#include "tool.h"

class QSpinBox;

// 橡皮
class RubberTool : public Tool
{
    Q_OBJECT
public:
    RubberTool();

    void HandelPress(EditorData* data, const Cell& cell,
                     const QString& color) override;
    void HandelMove(EditorData* data, const Cell& from,
                    const Cell& to, const QString& color) override;
    void HandelRelease(EditorData* data, const Cell& cellr) override;

private:
    void InitView();

    void HandleSpinBoxChange(int data);

private:
    QSpinBox* size_box_;

    int size_;
};

#endif // RUBBERTOOL_H
