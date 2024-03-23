#include "edit_view.h"

#include "edit_scene.h"
#include "editor_mediation.h"

#include <QOpenGLWidget>
#include <QScrollBar>

EditorView::EditorView(QWidget* parent)
  : QGraphicsView(parent)
  , current_scale_(1)
  , max_scale_(20000)
  , min_scale_(.01)
{
    // 使用GPU
    //    setViewport(new QOpenGLWidget());

    // 禁用QGraphicsView的滚动条
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // 布局策略
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setAlignment(Qt::AlignCenter);

    // 追踪鼠标
    setMouseTracking(true);
    setDragMode(NoDrag);

    ScaleScene(current_scale_);

    setFocusPolicy(Qt::StrongFocus);
}

EditorView::~EditorView()
{
}

// 缩放视图
void EditorView::ScaleScene(const double& new_scale)
{
    if (new_scale != current_scale_) {
        QTransform transform;
        transform.scale(new_scale, new_scale);
        setTransform(transform);
        current_scale_ = new_scale;
    }
}

// 沿着鼠标缩放视图
void EditorView::ScaleSceneByPoint(const double& new_scale, const QPointF& point)
{
    if (new_scale == current_scale_) {
        return;
    }

    // 获取当前鼠标相对于scene的位置;
    QPointF scene_pos = this->mapToScene(point.toPoint());

    // 获取当前鼠标位置相当于view大小的横纵比例;
    QPointF cur_scale(point.x() / viewport()->width(), point.y() / viewport()->height());

    QTransform matrix;
    matrix.scale(new_scale, new_scale);
    setTransform(matrix);
    current_scale_ = new_scale;

    // 将scene坐标转换为放大缩小后的坐标;
    QPointF view_point = matrix.map(scene_pos);
    // 通过滚动条控制view放大缩小后的展示scene的位置;
    horizontalScrollBar()->setValue(int(view_point.x() - viewport()->width() * cur_scale.x()));
    verticalScrollBar()->setValue(int(view_point.y() - viewport()->height() * cur_scale.y()));

    // 通知场景，缩放倍率改变
    //    mediation_->SetZoomFactor(current_scale_);
}

void EditorView::SetEditorMediation(EditorMediation* mediation)
{
    mediation_ = mediation;
}

// -------------------------------------------------------------
void EditorView::mousePressEvent(QMouseEvent* event)
{
    mediation_->MousePress(event);
}

void EditorView::mouseMoveEvent(QMouseEvent* event)
{
    mediation_->MouseMove(event);
}

void EditorView::mouseReleaseEvent(QMouseEvent* event)
{
    mediation_->MouseRelease(event);
}

void EditorView::wheelEvent(QWheelEvent* event)
{
    // 拦截
    // QGraphicsView::wheelEvent(event);

    const int delta = event->angleDelta().y();
    double new_scale = current_scale_;
    if (delta < 0) {
        new_scale *= 0.9;
        new_scale = new_scale < min_scale_ ? min_scale_ : new_scale;
    } else if (delta > 0) {
        new_scale *= 1.1;
        new_scale = new_scale > max_scale_ ? max_scale_ : new_scale;
    }
#if (QT_VERSION <= QT_VERSION_CHECK(5, 15, 0))
    ScaleSceneByPoint(new_scale, event->pos());
#else
    ScaleSceneByPoint(new_scale, event->position());
#endif
}

void EditorView::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event)
}

void EditorView::leaveEvent(QEvent* event)
{
    mediation_->MouseLeave(event);
}

void EditorView::keyPressEvent(QKeyEvent* event)
{
    mediation_->HandelKeyEvent(event);
}

void EditorView::enterEvent(QEvent* event)
{
    mediation_->MouseEnter(event);
}
