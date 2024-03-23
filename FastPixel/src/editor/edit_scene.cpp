#include "edit_scene.h"

#include <QGraphicsSceneMouseEvent>

EditorScene::EditorScene(QObject* parent)
  : QGraphicsScene(parent)
{
    ChangeSceneBgColor();
}

EditorScene::~EditorScene()
{
}

// 设置背景颜色
void EditorScene::ChangeSceneBgColor(const QColor& color)
{
    Q_UNUSED(color)
    QBrush bb;
    bb.setStyle(Qt::SolidPattern);
    bb.setColor(QColor(160, 160, 160));
    setBackgroundBrush(bb);
}

// 下发到Scene则不再拦截item处理
void EditorScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mousePressEvent(event);
    if (event->isAccepted()) {
        return;
    }
}

void EditorScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{

    QGraphicsScene::mouseMoveEvent(event);
    if (event->isAccepted()) {
        return;
    }
}

void EditorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseReleaseEvent(event);
    if (event->isAccepted()) {
        return;
    }
}
