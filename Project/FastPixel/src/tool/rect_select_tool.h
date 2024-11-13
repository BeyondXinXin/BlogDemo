#ifndef RECTSELECTTOOL_H
#define RECTSELECTTOOL_H

#include "tool.h"

class QPushButton;

class RectSelectTool : public Tool
{
    Q_OBJECT
public:
    RectSelectTool();

    void HandelPress(LayoutBuffer* lay_buffer, const Cell& cell) override;

    void HandelMove(LayoutBuffer* lay_buffer, const Cell& from,
                    const Cell& to) override;

    void HandelRelease(EditorData* data, const Cell& cellr) override;

private:
    void InitView();
    void HandlePushButton();

private:
    Cell first_cell_;
    Cell second_cell_;
    bool moveing_;

    QPushButton* btn_clear_;
};

#endif // RECTSELECTTOOL_H
