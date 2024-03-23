#include "PanelWidget.h"

#include <QBoxLayout>
#include <QFrame>
#include <QPainter>
#include <QScrollArea>

PanelWidget::PanelWidget(QWidget* parent)
	: QWidget(parent),
	m_ScrollArea(new QScrollArea(this)),
	m_ScrollAreaWidgetContents(new QWidget()),
	m_VerticalLayout(new QVBoxLayout(m_ScrollAreaWidgetContents)),
	m_Frame(new QFrame(m_ScrollAreaWidgetContents)),
	m_GridLayout(new QGridLayout(m_Frame)),
	m_Margin(0),
	m_Space(0),
	m_AutoWidth(false),
	m_AutoHeight(false)
{
	m_ScrollArea->setObjectName("scrollAreaMain");
	m_ScrollArea->setWidgetResizable(true);
	m_ScrollAreaWidgetContents->setGeometry(QRect(0, 0, 100, 100));
	m_Frame->setObjectName("frameMain");
	m_GridLayout->setSpacing(0);

	m_VerticalLayout->addWidget(m_Frame);
	m_ScrollArea->setWidget(m_ScrollAreaWidgetContents);
	m_ScrollArea->setStyleSheet("QScrollArea#scrollAreaMain,QFrame#frameMain{border:none;}");
}

void PanelWidget::resizeEvent(QResizeEvent*)
{
	m_ScrollArea->move(1, 1);
	m_ScrollArea->resize(this->size() - QSize(2, 2));
}

void PanelWidget::paintEvent(QPaintEvent* e)
{
	QWidget::paintEvent(e);
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	painter.setPen(QColor(39, 141, 255));
	painter.drawRect(1, 1, width() - 2, height() - 2);
}

QSize PanelWidget::sizeHint() const
{
	return QSize(300, 200);
}

QSize PanelWidget::minimumSizeHint() const
{
	return QSize(20, 20);
}

int PanelWidget::GetMargin() const
{
	return m_Margin;
}

int PanelWidget::GetSpace() const
{
	return m_Space;
}

bool PanelWidget::GetAutoWidth() const
{
	return m_AutoWidth;
}

bool PanelWidget::GetAutoHeight() const
{
	return m_AutoHeight;
}

QList<QWidget*> PanelWidget::GetWidgets()
{
	return m_Widgets;
}

int PanelWidget::GetColumnCount()
{
	return m_ColumnCount;
}

void PanelWidget::SetWidget(QList<QWidget*> widgets, int columnCount)
{
	m_Widgets = widgets;
	m_ColumnCount = columnCount;
	qDeleteAll(m_Frame->findChildren<QWidget*>());

	int row = 0;
	int column = 0;

	for (QWidget* widget : widgets) {
		m_GridLayout->addWidget(widget, row, column);
		column++;
		if (column % columnCount == 0) {
			row++;
			column = 0;
		}
	}

	row++;

	if (!m_AutoWidth) {
		QSpacerItem* hSpacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
		m_GridLayout->addItem(hSpacer, 0, m_GridLayout->columnCount());
	}

	if (!m_AutoHeight) {
		QSpacerItem* vSpacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_GridLayout->addItem(vSpacer, row, 0);
	}
}

void PanelWidget::SetMargin(int left, int top, int right, int bottom)
{
	m_GridLayout->setContentsMargins(left, top, right, bottom);
}

void PanelWidget::SetMargin(int margin)
{
	if (m_Margin != margin) {
		SetMargin(margin, margin, margin, margin);
	}
}

void PanelWidget::SetSpace(int space)
{
	if (m_Space != space) {
		m_GridLayout->setSpacing(space);
	}
}

void PanelWidget::SetAutoWidth(bool autoWidth)
{
	if (m_AutoWidth != autoWidth) {
		m_AutoWidth = autoWidth;
	}
}

void PanelWidget::SetAutoHeight(bool autoHeight)
{
	if (m_AutoHeight != autoHeight) {
		m_AutoHeight = autoHeight;
	}
}
