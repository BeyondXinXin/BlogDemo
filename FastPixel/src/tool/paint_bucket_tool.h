#ifndef PAINTBUCKETTOOL_H
#define PAINTBUCKETTOOL_H

#include "tool.h"

// 笔
class PaintBucketTool : public Tool
{
    Q_OBJECT
public:
    PaintBucketTool();

    void HandelPress(EditorData *data, const Cell &cell,
                     const QString &color) override;

private:
};

#endif // PAINTBUCKETTOOL_H
