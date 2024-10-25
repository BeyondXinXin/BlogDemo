#ifndef EDITORMEDIATION_H
#define EDITORMEDIATION_H

#include "data/cell.h"
#include "tool/tool.h"

#include <QMouseEvent>

class EditorView;
class EditorData;
class EditorScene;

class LayoutTransparentBG;
class LayoutPixel;
class LayoutBuffer;
class LayoutHover;

class EditorMediation
{
public:
    EditorMediation(int uid);
    ~EditorMediation();

    QWidget* GetEditorView();

    int Width() const;
    int Height() const;

    void ViewCamReset();

    void HandelPress(const Cell& cell, const QString& color_name);
    void HandelMove(const Cell& from, const Cell& to, const QString& color_name);
    void HandelRelease(const Cell& cell);

    void HandelKeyEvent(QKeyEvent* event);

    void MousePress(QMouseEvent* event);
    void MouseMove(QMouseEvent* event);
    void MouseRelease(QMouseEvent* event);
    void MouseEnter(QEvent* event);
    void MouseLeave(QEvent* event);

    // 修改颜色
    void ChangePSColor(const QString& pcolor, const QString& scolor);
    void MoveBgLayout(const QPointF& delta);

    // 处理工具事件
    void HandelToolEvent(const ToolEvent& data);

    void Redo();
    void Undo();

    void SaveFile();

private:
    QPointF ViewPosToScene(const QPoint& pos);

private:
    short cur_uid_;

    EditorView* view_;
    EditorScene* scene_;
    EditorData* data_;

    LayoutTransparentBG* lay_tp_bg_;
    LayoutPixel* lay_pixel_;
    LayoutBuffer* lay_buffer_;
    LayoutHover* lay_hover_;

    QString primary_color_;
    QString secondary_color_;

    Cell last_cell_;
    QPointF prev_mid_mouse_pos_;

    QString file_path_;
};

#endif // EDITORMEDIATION_H
