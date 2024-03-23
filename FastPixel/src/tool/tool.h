#ifndef TOOL_H
#define TOOL_H

#include "cell.h"
#include <QAction>

class EditorData;
class LayoutBuffer;

enum ToolEvent
{
    KEmpty = -1,
    KRectSelect_Clear = 0,
};

class Tool : public QAction
{
    Q_OBJECT
public:
public:
    Tool();
    ~Tool() override;

    virtual void HandelPress(EditorData* data, const Cell& cell,
                             const QString& color);

    virtual void HandelMove(EditorData* data, const Cell& from,
                            const Cell& to, const QString& color);

    virtual void HandelRelease(EditorData* data, const Cell& cell);

    virtual void HandelPress(LayoutBuffer* lay_buffer, const Cell& cell);

    virtual void HandelMove(LayoutBuffer* lay_buffer, const Cell& from,
                            const Cell& to);

    QCursor ToolCursor() const;

    bool UseBuffer() const;
    bool RealTimeUD() const;

    QList<QWidget*> GetToolAttributeWid();

Q_SIGNALS:
    void SgnToolEvent(const ToolEvent& data);

protected:
    QCursor cursor_; // 工具光标

    bool use_buffer_; // 是否 使用缓冲图层
    bool real_time_update_; // 鼠标移动时是否 实时更新图层

    QList<QWidget*> attribute_wids_;
};

#endif // TOOL_H
