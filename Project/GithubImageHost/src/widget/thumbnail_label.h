#ifndef THUMBNAILLABEL_H
#define THUMBNAILLABEL_H

#include <QItemSelection>
#include <QMenu>
#include <QPointer>
#include <QPushButton>

#include "util/github_image_host.h"

class ThumbnailLabel : public QWidget
{
    Q_OBJECT

public:
    explicit ThumbnailLabel(const GHImage &img, QWidget *parent = nullptr);
    ~ThumbnailLabel() override;

Q_SIGNALS:
    void SgnDeleteImg(const QString &github_path, const QString &sha);

protected:
    void paintEvent(QPaintEvent *) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

    virtual void contextMenuEvent(QContextMenuEvent *e) override;

private:
    void Initial();
    void DrawBg(QPainter *painter);

private:
    GHImage ghimage_; // 实例化一个，增加开销但是省很多事。
    bool hover_ { false };

    QPointer<QMenu> context_menu_;
    QItemSelection selection_;
};

#endif // THUMBNAILLABEL_H
