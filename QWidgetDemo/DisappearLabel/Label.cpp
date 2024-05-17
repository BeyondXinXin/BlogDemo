// https://github.com/Greedysky/TTKWidgetTools

#include "Label.h"
#include "Label.h"

#include <QPainter>
#include <QDebug>
#include <QPropertyAnimation>

Label::Label(QWidget* parent)
	: QLabel(parent),
	m_Font(font())
{
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_DeleteOnClose);

	SetFontSize(15);
	SetFontMargin(20, 20);

	connect(&m_Timer, &QTimer::timeout, this, &Label::CloseAnimation);
	m_Timer.setInterval(1000);
	m_Timer.start();
}

Label::Label(const QString& text, QWidget* parent)
	: Label(parent)
{
	SetText(text);
}

Label::~Label()
{
	m_Timer.stop();
}

void Label::SetFontMargin(int height, int width)
{
	m_Margin.setX(height);
	m_Margin.setY(width);
}

int Label::TimerInterval() const
{
	return m_Timer.interval();
}

void Label::SetFontSize(int size)
{
	m_Font.setPointSize(size);
	setFont(m_Font);
}

void Label::Popup(QWidget* parent)
{
	if (parent) {
		const QPoint& globalPoint = parent->mapToGlobal(QPoint(0, 0));
		move(
			globalPoint.x() + (parent->width() - width()) / 2,
			globalPoint.y() + (parent->height() - height()) / 2);
	}
	show();
}

void Label::SetText(const QString& text)
{
	const QFontMetrics ftm(m_Font);
	setFixedSize(ftm.horizontalAdvance(text) + m_Margin.x(), ftm.height() + m_Margin.y());
	QLabel::setText(text);
}

void Label::CloseAnimation()
{
	m_Timer.stop();

	QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity", this);
	animation->setDuration(1000);
	animation->setStartValue(1);
	animation->setEndValue(0);
	animation->start();

	connect(animation, &QPropertyAnimation::finished, this, &Label::close);
	connect(animation, &QPropertyAnimation::finished, this, &Label::SgnFinished);
}

void Label::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(Qt::NoPen);
	painter.setBrush(QColor(0, 0, 0, 175));
	painter.drawRoundedRect(rect(), 6, 6);

	painter.setPen(QColor(255, 255, 255));
	painter.drawText(rect(), Qt::AlignCenter, text());
}
