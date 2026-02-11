#ifndef QOPENCVWIDGET_H
#define QOPENCVWIDGET_H

#include "stable.h"

class QOpencvPixmapItem;
class QOpencvScene;
class QOpencvView;

class QOpencvWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QOpencvWidget(QWidget *parent = nullptr);
    ~QOpencvWidget();
    void SetSurface(const QImage value);
    QImage GetSurface();

private:
    void Initial();

private:
    QGridLayout *gridLayout;
    QOpencvView *graphicsView;
    QOpencvPixmapItem *opencv_pixmap_;
    QOpencvScene *opencv_scene_;
    QImage img_;
};

class QOpencvPixmapItem : public QGraphicsPixmapItem
{
public:
    explicit QOpencvPixmapItem(QGraphicsPixmapItem *parent = nullptr);
    ~QOpencvPixmapItem();
};

class QOpencvScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit QOpencvScene(QGraphicsScene *parent = nullptr);
    ~QOpencvScene();
};

class QOpencvView : public QGraphicsView
{
public:
    explicit QOpencvView(QWidget *parent = nullptr);
    ~QOpencvView();

protected:
    void wheelEvent(QWheelEvent *event);
};

#endif // QOPENCVWIDGET_H
