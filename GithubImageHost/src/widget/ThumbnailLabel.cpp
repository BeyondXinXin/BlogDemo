#include "ThumbnailLabel.h"

#include <QApplication>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QPainter>

ThumbnailLabel::ThumbnailLabel(const GHImage& img, QWidget* parent)
	: QWidget(parent)
	, m_GHImage(img)
{
	Initialize();
}

ThumbnailLabel::~ThumbnailLabel()
{
	m_ContextMenu->deleteLater();
}

void ThumbnailLabel::paintEvent(QPaintEvent* e)
{
	QWidget::paintEvent(e);
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	DrawBackground(&painter);
}

void ThumbnailLabel::enterEvent(QEvent* event)
{
	m_Hover = true;
	QWidget::enterEvent(event);
	this->update();
}

void ThumbnailLabel::leaveEvent(QEvent* event)
{
	m_Hover = false;
	QWidget::leaveEvent(event);
	this->update();
}

void ThumbnailLabel::contextMenuEvent(QContextMenuEvent* e)
{
	m_ContextMenu->popup(e->globalPos());
}

void ThumbnailLabel::Initialize()
{
	setFixedSize(GitHubImageHost::m_ImageSize + QSize(14, 40));

	// 右键菜单
	this->m_ContextMenu = new QMenu(this);
	m_ContextMenu->addAction(u8"删除", this, [this]() {
		emit SgnDeleteImg(m_GHImage.githubPath, m_GHImage.sha);
		});
	m_ContextMenu->addSeparator();
	m_ContextMenu->addAction(u8"复制md链接", this, [this]() {
		QClipboard* clip = QApplication::clipboard();
		clip->setText(m_GHImage.downloadUrl);
		});
	m_ContextMenu->addAction(u8"复制下载链接", this, [this]() {
		QClipboard* clip = QApplication::clipboard();
		clip->setText(m_GHImage.downloadUrl);
		});
}

void ThumbnailLabel::DrawBackground(QPainter* painter)
{
	painter->save();

	painter->setBrush(QColor(52, 52, 52, 0));
	painter->drawRect(rect());

	if (m_Hover) {
		painter->setPen(QPen(QColor(44, 166, 224), 2, Qt::SolidLine));
	}
	else {
		painter->setPen(QPen(QColor(212, 212, 212), 2, Qt::SolidLine));
	}

	painter->drawRect(1, 1, width() - 2, height() - 2);

	auto size = GitHubImageHost::m_ImageSize;
	const auto wid = (size.width() - m_GHImage.pix.width()) / 2;
	painter->drawImage(QRect(7, 10, size.width(), size.height()), m_GHImage.pix);
	painter->translate(0, size.height());
	
	QFont font;
	font.setPointSize(9);
	painter->setFont(font);
	painter->setPen(QPen(QColor(212, 212, 212), 2, Qt::SolidLine));
	painter->drawText(QPoint(10, 30), m_GHImage.name);

	painter->restore();
}
