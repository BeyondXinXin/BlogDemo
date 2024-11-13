#include "tool.h"

Tool::Tool()
{
    setCheckable(true);
    use_buffer_ = false;
    real_time_update_ = false;
}

Tool::~Tool()
{
}

QCursor Tool::ToolCursor() const
{
    return cursor_;
}

bool Tool::UseBuffer() const
{
    return use_buffer_;
}

bool Tool::RealTimeUD() const
{
    return real_time_update_;
}

QList<QWidget*> Tool::GetToolAttributeWid()
{
    return attribute_wids_;
}

void Tool::HandelPress(EditorData* data, const Cell& cell, const QString& color)
{
    Q_UNUSED(data)
    Q_UNUSED(cell)
    Q_UNUSED(color)
}

void Tool::HandelMove(EditorData* data, const Cell& from, const Cell& to, const QString& color)
{
    Q_UNUSED(data)
    Q_UNUSED(from)
    Q_UNUSED(to)
    Q_UNUSED(color)
}

void Tool::HandelRelease(EditorData* data, const Cell& cell)
{
    Q_UNUSED(data)
    Q_UNUSED(cell)
}

void Tool::HandelPress(LayoutBuffer* lay_buffer, const Cell& cell)
{
    Q_UNUSED(lay_buffer)
    Q_UNUSED(cell)
}

void Tool::HandelMove(LayoutBuffer* lay_buffer, const Cell& from, const Cell& to)
{
    Q_UNUSED(lay_buffer)
    Q_UNUSED(from)
    Q_UNUSED(to)
}
