#include "editor_mediation.h"

#include "edit_scene.h"
#include "edit_view.h"
#include "editor_data.h"
#include "layout/layout_bg.h"
#include "layout/layout_buffer.h"
#include "layout/layout_hover.h"
#include "layout/layout_pixel.h"
#include "qglobal.h"
#include "qobject.h"
#include "singleton.h"
#include "tool_manager.h"

#include <QApplication>
#include <QFileDialog>

EditorMediation::EditorMediation(int uid)

  : cur_uid_(uid)

  , view_(new EditorView())
  , scene_(new EditorScene())
  , data_(new EditorData(32, 32, 8))

  , lay_tp_bg_(new LayoutTransparentBG())
  , lay_pixel_(new LayoutPixel())
  , lay_buffer_(new LayoutBuffer())
  , lay_hover_(new LayoutHover())

{

    // bg - pixel - buffer - hover

    // 背景层设置
    lay_tp_bg_->Resize(Width(), Height());
    lay_tp_bg_->setPos(0, 0);

    // 像素层设置
    lay_pixel_->Resize(Width(), Height());
    lay_pixel_->setPos(0, 0);
    lay_pixel_->SetData(data_);
    lay_pixel_->setParentItem(lay_tp_bg_);

    // 选择缓冲层设置
    lay_buffer_->Resize(Width(), Height());
    lay_buffer_->setPos(0, 0);
    lay_buffer_->SetData(data_);
    lay_buffer_->setParentItem(lay_pixel_);

    // hover层设置
    lay_hover_->Resize(Width(), Height());
    lay_hover_->setPos(0, 0);
    lay_hover_->SetData(data_);
    lay_hover_->setParentItem(lay_pixel_);

    // 场景设置
    scene_->setSceneRect(0, 0, Width(), Height());
    scene_->addItem(lay_tp_bg_);

    // 视图设置
    view_->setScene(scene_);
    view_->SetEditorMediation(this);

    file_path_ = "";
}

// 返回视图，用来增加到主窗口
QWidget* EditorMediation::GetEditorView()
{
    return view_;
}

// 场景宽度
int EditorMediation::Width() const
{
    return data_->Width();
}

// 场景高度
int EditorMediation::Height() const
{
    return data_->Height();
}

// 视图复位
void EditorMediation::ViewCamReset()
{
    const double new_scale = view_->width() > view_->height()
      ? view_->height() / view_->sceneRect().height()
      : view_->width() / view_->sceneRect().width();
    view_->ScaleScene(new_scale * 0.9);
}

// 处理鼠标按下（根据工具）
void EditorMediation::HandelPress(
  const Cell& cell, const QString& color_name)
{
    auto* tool = Singleton<ToolManager>::Inst().CurToolBar();
    bool use_buffer = tool->UseBuffer();
    bool rt_ud = tool->RealTimeUD();
    if (!use_buffer) {
        tool->HandelPress(data_, cell, color_name);
        if (rt_ud) {
            //            lay_pixel_->UpdataBg();
        }
        lay_pixel_->update();
    } else {
        tool->HandelPress(lay_buffer_, cell);
    }
}

// 处理鼠标移动（根据工具）
void EditorMediation::HandelMove(
  const Cell& from, const Cell& to, const QString& color_name)
{
    auto* tool = Singleton<ToolManager>::Inst().CurToolBar();
    bool use_buffer = tool->UseBuffer();
    bool rt_ud = tool->RealTimeUD();
    if (!use_buffer) {
        tool->HandelMove(data_, from, to, color_name);
        if (rt_ud) {
            //            lay_pixel_->UpdataBg();
        }
        lay_pixel_->update();
    } else {
        tool->HandelMove(lay_buffer_, from, to);
        scene_->update();
    }
}

// 处理鼠标松开（根据工具）
void EditorMediation::HandelRelease(const Cell& cell)
{
    auto* tool = Singleton<ToolManager>::Inst().CurToolBar();
    bool use_buffer = tool->UseBuffer();
    if (!use_buffer) {
        tool->HandelRelease(data_, cell);
        //        lay_pixel_->UpdataBg();
        lay_pixel_->update();
    } else {
        tool->HandelRelease(data_, cell);
        //        lay_pixel_->UpdataBg();
        lay_pixel_->update();
    }
}

// 处理键盘事件
void EditorMediation::HandelKeyEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Escape: {
        lay_buffer_->HandelKeyEvent(event);
        break;
    }
    default:
        break;
    }
}

// 处理鼠标按下
void EditorMediation::MousePress(QMouseEvent* event)
{
    lay_hover_->SetHoverEnable(false);

    auto mouse_buttons = QApplication::mouseButtons();
    if (mouse_buttons & Qt::LeftButton) {
        auto pos = ViewPosToScene(event->pos());
        last_cell_ = data_->PointToCell(pos);
        HandelPress(last_cell_, primary_color_);
    } else if (mouse_buttons & Qt::RightButton) {
        auto pos = ViewPosToScene(event->pos());
        last_cell_ = data_->PointToCell(pos);
        HandelPress(last_cell_, secondary_color_);
    } else if (mouse_buttons & Qt::MiddleButton) { // 只下发中键拦截
        prev_mid_mouse_pos_ = event->pos();
    } else {
    }

    event->accept();
}

// 处理鼠标移动
void EditorMediation::MouseMove(QMouseEvent* event)
{
    auto mouse_buttons = QApplication::mouseButtons();

    if (mouse_buttons & Qt::LeftButton) { // 鼠标左键

        auto pos = ViewPosToScene(event->pos());
        Cell new_cell = data_->PointToCell(pos);
        if (new_cell == last_cell_) {
            return;
        }
        HandelMove(last_cell_, new_cell, primary_color_);
        last_cell_ = new_cell;

    } else if (mouse_buttons & Qt::RightButton) { // 鼠标右键

        auto pos = ViewPosToScene(event->pos());
        Cell new_cell = data_->PointToCell(pos);
        if (new_cell == last_cell_) {
            return;
        }
        HandelMove(last_cell_, new_cell, secondary_color_);
        last_cell_ = new_cell;

    } else if (mouse_buttons & Qt::MiddleButton) { // 鼠标中键,移动画布
        view_->setDragMode(QGraphicsView::NoDrag);
        scene_->clearSelection();
        if (scene_->mouseGrabberItem()) {
            scene_->mouseGrabberItem()->ungrabMouse();
        }

        QPointF delta_pos = event->pos() - prev_mid_mouse_pos_;
        MoveBgLayout(delta_pos);
        prev_mid_mouse_pos_ = event->pos();
    } else { // 鼠标移动时无按键

        if (lay_hover_->HoverEnable()) { // 是否需要hover事件
            auto pos = ViewPosToScene(event->pos());
            Cell new_cell = data_->PointToCell(pos);
            if (new_cell == last_cell_) {
                return;
            }
            lay_hover_->MouseHover(new_cell);
            last_cell_ = new_cell;
        }
    }

    event->accept();
}

// 处理鼠标松开
void EditorMediation::MouseRelease(QMouseEvent* event)
{
    lay_hover_->SetHoverEnable(true);
    if (event->button() == Qt::LeftButton
        || event->button() == Qt::RightButton) {
        auto pos = ViewPosToScene(event->pos());
        HandelRelease(data_->PointToCell(pos));
    } else if (event->button() == Qt::MiddleButton) {

    } else {
    }
    event->accept();
}

// 鼠标移入场景
void EditorMediation::MouseEnter(QEvent* event)
{
    Q_UNUSED(event)
    //    auto* tool = Singleton<ToolManager>::Inst().CurToolBar();
    //    view_->setCursor(tool->ToolCursor());
}

// 鼠标移出场景
void EditorMediation::MouseLeave(QEvent* event)
{
    Q_UNUSED(event)
}

// 修改主色副色
void EditorMediation::ChangePSColor(const QString& pcolor, const QString& scolor)
{
    primary_color_ = pcolor;
    secondary_color_ = scolor;
}

// 移动场景
void EditorMediation::MoveBgLayout(const QPointF& delta)
{
    lay_tp_bg_->setPos(lay_tp_bg_->pos() + delta / (view_->GetCurrentScale()));
    lay_pixel_->update();
    lay_buffer_->update();
}

// 处理工具事件
void EditorMediation::HandelToolEvent(const ToolEvent& data)
{
    switch (data) {
    case KRectSelect_Clear: {
        lay_buffer_->ClearBuffer();
        break;
    }
    default:
        break;
    }
}

void EditorMediation::Redo()
{
    data_->Redo();
    lay_pixel_->update();
}

void EditorMediation::Undo()
{
    data_->Undo();
    lay_pixel_->update();
}

// 保持图片
void EditorMediation::SaveFile()
{
    if (file_path_.isEmpty()) {
        file_path_ = QFileDialog::getSaveFileName(0, QString("select file"), "./", ".png");

        if (file_path_.isEmpty()) {
            return;
        }
    }

    data_->GetCurrentPix().save(file_path_);
}

// 计算视图点到场景点
QPointF EditorMediation::ViewPosToScene(const QPoint& pos)
{
    return view_->mapToScene(pos) - lay_tp_bg_->pos();
}
