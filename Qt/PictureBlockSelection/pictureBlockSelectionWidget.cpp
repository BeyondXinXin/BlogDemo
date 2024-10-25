#include "PictureBlockSelectionWidget.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPainter>

const static QBrush brush_bg(QColor(0, 0, 0));
const static QBrush brush_select_block(QColor(200, 200, 200, 150));
const static QPen   pen_select_frame(Qt::green, 10, Qt::SolidLine);
const static QPen   pen_frame(Qt::gray, 5, Qt::SolidLine);
const static QPen   pen_name(Qt::red, 10);
const static QPen   pen_bg_frame(Qt::gray, 4, Qt::SolidLine);
const static QPoint offset(20, 20);
const static QSize  image_size(1024, 1024);

PictureBlockSelectionWidget::PictureBlockSelectionWidget(QWidget* parent)
	: QWidget(parent)
{
	this->Initial();
}

PictureBlockSelectionWidget::~PictureBlockSelectionWidget()
{}

void PictureBlockSelectionWidget::AddBlock(const QList<QPoint>& path, const QString& name)
{
	m_BlockPathHash.insert(m_BlockPathHash.size(), path);
	m_TagList << name;
}

void PictureBlockSelectionWidget::SetCurrentBlock(const int& id)
{
	if ((id >= 0) || (id < m_BlockPathHash.size())) {
		m_CurBlock = id;
	}
	this->update();
}

void PictureBlockSelectionWidget::SetHoverBlock(const int& id)
{
	if ((id >= 0) || (id < m_BlockPathHash.size())) {
		m_HoverBlock = id;
	}
	this->update();
}

void PictureBlockSelectionWidget::ClearAllBlock()
{
	m_TagList.clear();
	m_BlockPathHash.clear();
}

void PictureBlockSelectionWidget::SetBreakGrandImage(const QImage& img)
{
	this->m_BgImage = img.scaled(image_size, Qt::KeepAspectRatio);
}

qreal PictureBlockSelectionWidget::GetImageSizeWidth()
{
	return image_size.width();
}

qreal PictureBlockSelectionWidget::GetImageSizeHeigh()
{
	return image_size.height();
}

void PictureBlockSelectionWidget::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	DrawBg(&painter);
	painter.translate(offset);
	DrawBgImage(&painter);
	DrawFrame(&painter);
	DrawSelectBlock(&painter);
	DrawSelectFrame(&painter);
	DrawBlockName(&painter);
	painter.translate(-offset);
}

void PictureBlockSelectionWidget::mousePressEvent(QMouseEvent* event)
{
	bool contains = false;
	foreach(auto key, m_BlockPathHash.keys())
	{
		QPolygon polygon; // 多边形
		foreach(auto var, m_BlockPathHash.value(key))
		{
			polygon << var;
		}
		if (polygon.containsPoint(event->pos() - offset, Qt::OddEvenFill)) {
			m_CurBlock = key;
			contains = true;
			break;
		}
	}
	if (!contains) {
		m_CurBlock = -1;
	}
	this->update();
	emit SgnCurrentBlockChange(m_CurBlock);
	QWidget::mousePressEvent(event);
}

void PictureBlockSelectionWidget::mouseMoveEvent(QMouseEvent* event)
{
	bool contains = false;
	foreach(auto key, m_BlockPathHash.keys())
	{
		QPolygon polygon; // 多边形
		foreach(auto var, m_BlockPathHash.value(key))
		{
			polygon << var;
		}
		if (polygon.containsPoint(event->pos() - offset, Qt::OddEvenFill)) {
			m_HoverBlock = key;
			contains = true;
			break;
		}
	}
	if (!contains) {
		m_HoverBlock = -1;
	}
	this->update();
	emit SgnHoverBlockChange(m_HoverBlock);
	QWidget::mouseMoveEvent(event);
}

void PictureBlockSelectionWidget::Initial()
{
	m_HoverBlock = -1;
	m_CurBlock = -1;
	setMouseTracking(true);
}

void PictureBlockSelectionWidget::DrawBg(QPainter* painter) // 黑色底色和边框
{
	painter->save();
	painter->setBrush(brush_bg);
	painter->drawRect(rect());
	painter->setPen(pen_bg_frame);
	painter->drawRect(rect());
	painter->restore();
}

void PictureBlockSelectionWidget::DrawBgImage(QPainter* painter)
{
	painter->save();
	painter->drawImage(0, 0, m_BgImage);
	painter->restore();
}

void PictureBlockSelectionWidget::DrawFrame(QPainter* painter)
{
	painter->save();
	painter->setPen(pen_frame);
	foreach(auto var, m_BlockPathHash.values())
	{
		QPolygon polygon; // 多边形
		foreach(auto var, var)
		{
			polygon << var;
		}
		painter->drawConvexPolygon(polygon);
	}
	painter->restore();
}

void PictureBlockSelectionWidget::DrawSelectFrame(QPainter* painter)
{
	painter->save();
	painter->setPen(pen_select_frame);
	if (m_BlockPathHash.keys().contains(m_HoverBlock)) {
		QPolygon polygon; // 多边形
		foreach(auto var, m_BlockPathHash.value(m_HoverBlock))
		{
			polygon << var;
		}
		painter->drawConvexPolygon(polygon);
	}
	painter->restore();
}

void PictureBlockSelectionWidget::DrawSelectBlock(QPainter* painter)
{
	painter->save();
	painter->setBrush(brush_select_block);
	if (m_BlockPathHash.keys().contains(m_CurBlock)) {
		QPolygon polygon; // 多边形
		foreach(auto var, m_BlockPathHash.value(m_CurBlock))
		{
			polygon << var;
		}
		painter->drawPolygon(polygon);
	}
	painter->restore();
}

void PictureBlockSelectionWidget::DrawBlockName(QPainter* painter)
{
	painter->save();
	QFont font;
	font.setPointSize(20);
	painter->setPen(pen_name);
	painter->setFont(font);
	foreach(auto key, m_BlockPathHash.keys())
	{
		painter->drawText(m_BlockPathHash.value(key).at(0) + QPoint(20, 40), m_TagList.at(key));
	}
	painter->restore();
}
