#ifndef PANELWIDGET_H
#define PANELWIDGET_H

#include <QWidget>

class QScrollArea;
class QFrame;
class QVBoxLayout;
class QGridLayout;

class PanelWidget : public QWidget
{
	Q_OBJECT

public:
	explicit PanelWidget(QWidget* parent = nullptr);

public:
	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;

	int GetMargin() const;
	int GetSpace() const;
	bool GetAutoWidth() const;
	bool GetAutoHeight() const;

	QList<QWidget*> GetWidgets();
	int GetColumnCount();

public Q_SLOTS:
	void SetWidget(QList<QWidget*> widgets, int columnCount);
	void SetMargin(int left, int top, int right, int bottom);
	void SetMargin(int margin);
	void SetSpace(int space);
	void SetAutoWidth(bool autoWidth);
	void SetAutoHeight(bool autoHeight);

protected:
	void resizeEvent(QResizeEvent*) override;
	void paintEvent(QPaintEvent*) override;

private:
	QScrollArea* m_ScrollArea; //滚动区域
	QWidget* m_ScrollAreaWidgetContents; //滚动区域载体
	QFrame* m_Frame; //放置设备的框架,自动变宽变高
	QVBoxLayout* m_VerticalLayout; //设备面板总布局
	QGridLayout* m_GridLayout; //设备表格布局

	int m_Margin; //边距
	int m_Space; //设备之间的间隔
	bool m_AutoWidth; //宽度自动拉伸
	bool m_AutoHeight; //高度自动拉伸

	QList<QWidget*> m_Widgets; //设备面板对象集合
	int m_ColumnCount; //面板列数
};

#endif // PANELWIDGET_H
