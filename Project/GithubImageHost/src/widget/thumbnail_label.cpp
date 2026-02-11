#include "thumbnail_label.h"

#include <QApplication>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QPainter>

ThumbnailLabel::ThumbnailLabel(const GHImage &img, QWidget *parent)
  : QWidget(parent)
  , ghimage_(img)
{
    Initial();
}

ThumbnailLabel::~ThumbnailLabel()
{
    context_menu_->deleteLater();
}

void ThumbnailLabel::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    DrawBg(&painter);
}

void ThumbnailLabel::enterEvent(QEvent *event)
{
    hover_ = true;
    QWidget::enterEvent(event);
    this->update();
}

void ThumbnailLabel::leaveEvent(QEvent *event)
{
    hover_ = false;
    QWidget::leaveEvent(event);
    this->update();
}

void ThumbnailLabel::contextMenuEvent(QContextMenuEvent *e)
{
    context_menu_->popup(e->globalPos());
}

void ThumbnailLabel::Initial()
{
    setFixedSize(GitHubImageHost::img_size_ + QSize(14, 40));

    // 右键菜单
    this->context_menu_ = new QMenu(this);
    context_menu_->addAction(u8"删除", this, [=]() {
        emit SgnDeleteImg(ghimage_.github_path, ghimage_.sha);
    });
    context_menu_->addSeparator();
    context_menu_->addAction(u8"复制md链接", this, [=]() {
        QClipboard *clip = QApplication::clipboard();
        clip->setText(ghimage_.download_url);
    });
    context_menu_->addAction(u8"复制下载链接", this, [=]() {
        QClipboard *clip = QApplication::clipboard();
        clip->setText(ghimage_.download_url);
    });
}

void ThumbnailLabel::DrawBg(QPainter *painter)
{
    painter->save();

    painter->setBrush(QColor(52, 52, 52, 0));
    painter->drawRect(rect());

    if (hover_) {
        painter->setPen(QPen(QColor(44, 166, 224), 2, Qt::SolidLine));
    } else {
        painter->setPen(QPen(QColor(212, 212, 212), 2, Qt::SolidLine));
    }

    painter->drawRect(1, 1, width() - 2, height() - 2);

    const auto wid = (GitHubImageHost::img_size_.width() - ghimage_.pix.width()) / 2;
    painter->drawImage(QPoint(7 + wid, 10), ghimage_.pix);

    painter->translate(0, GitHubImageHost::img_size_.height());
    QFont font;
    font.setPointSize(9);
    painter->setFont(font);
    painter->setPen(QPen(QColor(212, 212, 212), 2, Qt::SolidLine));
    painter->drawText(QPoint(10, 30), ghimage_.name);

    painter->restore();
}
