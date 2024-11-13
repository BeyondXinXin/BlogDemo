#ifndef DOUBLESLIDER_H
#define DOUBLESLIDER_H

#include <QSlider>

class DoubleSlider : public QSlider {
	Q_OBJECT

public:

	DoubleSlider(double min, double max, QWidget* parent = nullptr);
	~DoubleSlider() override = default;

	double GetMin() const;
	double GetMax() const;
	double GetCurrentMin() const;
	double GetCurrentMax() const;

public Q_SLOTS:

	void SetCurrentMin(double currentMinToSet);
	void SetCurrentMax(double currentMaxToSet);

Q_SIGNALS:

	void SgnCurrentMinChanged(double currentMin);
	void SgnCurrentMaxChanged(double currentMax);
	void SgnCurrentValueChanged(double wl, double ww);

protected:

	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void changeEvent(QEvent* event) override;

private:

	enum class Handle : unsigned short int
	{
		NONE,
		LEFT,
		RIGHT,
		Middle
	};

private:

	int GetHandlePosition(Handle handle) const;
	void DrawSliderBarText(QPainter& painter) const;
	void DrawSliderBar(QPainter& painter) const;
	void DrawSliderBarBetweenHandles(QPainter& painter) const;
	QStyleOptionSlider GetHandleStyle(bool moving, int position) const;
	void DrawHandles(QPainter& painter) const;
	int GetMousePosition(QMouseEvent* event) const;
	bool MouseIsOnHandle(int mouse_pos_x, int handle_pos) const;
	int GetHandleMiddlePosX(int handle_pos, int handle_width, int span) const;
	void SetHandleRect();
	double GetNormalizedValue(double value);

private:

	const double m_Min;
	const double m_Max;
	double m_CurrentMin { 0. };
	double m_CurrentMax { 0. };
	double m_CurrentMiddle { 0. };
	double m_LastEmittedMin { 0. };
	double m_LastEmittedMax { 0. };
	double m_LastEmittedMiddle { 0. };

	QRect m_HandleRect;
	Handle m_Moving { Handle::NONE };

	static constexpr int MAX_PERCENT { 100 };
	static constexpr double BAR_HEIGHT_RATIO { 1. / 10. };
};

#endif // DOUBLESLIDER_H
