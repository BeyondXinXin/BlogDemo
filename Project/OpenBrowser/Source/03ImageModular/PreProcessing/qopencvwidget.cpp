#pragma execution_character_set("utf-8")
// 01 Frame includes
#include "qopencvwidget.h"

QOpencvWidget::QOpencvWidget(QWidget *parent)
  : QWidget(parent)
{
    this->Initial();
}

QOpencvWidget::~QOpencvWidget()
{
}

void QOpencvWidget::SetSurface(const QImage value)
{
    int tmp_height = img_.height() - value.height();
    int tmp_width = img_.width() - value.width();
    img_ = value;
    if (tmp_height > 0 || tmp_width > 0) {
        opencv_pixmap_->setOffset(tmp_width / 2, tmp_height / 2);
    } else if (tmp_height < 0 || tmp_width < 0) {
        opencv_pixmap_->setOffset(0, 0);
    }
    opencv_pixmap_->setPixmap(QPixmap::fromImage(img_));
}

QImage QOpencvWidget::GetSurface()
{
    return img_;
}

void QOpencvWidget::Initial()
{
    this->setObjectName(QStringLiteral("QOpencvWidget"));
    gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    graphicsView = new QOpencvView(this);
    gridLayout->addWidget(graphicsView);
    opencv_scene_ = new QOpencvScene;
    opencv_pixmap_ = new QOpencvPixmapItem;
    graphicsView->setScene(opencv_scene_);
    opencv_scene_->addItem(opencv_pixmap_);
    graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    graphicsView->show();
}

QOpencvPixmapItem::QOpencvPixmapItem(QGraphicsPixmapItem *parent)
  : QGraphicsPixmapItem(parent)
{
}

QOpencvPixmapItem::~QOpencvPixmapItem()
{
}
QOpencvScene::QOpencvScene(QGraphicsScene *parent)
  : QGraphicsScene(parent)
{
}

QOpencvScene::~QOpencvScene()
{
}

QOpencvView::QOpencvView(QWidget *parent)
  : QGraphicsView(parent)
{
}

QOpencvView::~QOpencvView()
{
}

void QOpencvView::wheelEvent(QWheelEvent *event)
{
    if (event->delta() > 0) {
        this->scale(1.1, 1.1);
    } else {
        this->scale(0.9, 0.9);
    }
}
