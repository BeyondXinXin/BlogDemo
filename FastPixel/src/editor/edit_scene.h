#ifndef EDITSCENE_H
#define EDITSCENE_H

#include <QGraphicsScene>

class EditorScene : public QGraphicsScene
{
    Q_OBJECT
public:
    EditorScene(QObject* parent = nullptr);
    ~EditorScene() override;

    void ChangeSceneBgColor(const QColor& color = QColor(160, 160, 160));

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

public:
};

#endif // EDITSCENE_H
