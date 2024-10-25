#ifndef BullsEye_H
#define BullsEye_H

#include <QWidget>
#include <QComboBox>


class BullsEye : public QWidget {
	Q_OBJECT

	Q_PROPERTY(QList<QVector<double> >current_value READ GetCurrentValue WRITE SetCurrentValue)
	Q_PROPERTY(QColor bg_color_start READ GetBgColorStart WRITE SetBgColorStart)
	Q_PROPERTY(QColor bg_color_end READ GetBgColorEnd WRITE SetBgColorEnd)
	Q_PROPERTY(QColor bar_color_start READ GetBarColorStart WRITE SetBarColorStart)
	Q_PROPERTY(QColor bar_color_end READ GetBarColorEnd WRITE SetBarColorEnd)

public:

	explicit BullsEye(QWidget* parent = nullptr);
	~BullsEye();

public:

	QColor GetBgColorStart()                const;
	QColor GetBgColorEnd()                  const;
	QColor GetBarColorStart()               const;
	QColor GetBarColorEnd()                 const;
	QList<QVector<double> >GetCurrentValue()       const;

	QRgb GetGradient(const double value);
	void SetBgColorStart(const QColor& bg_color_start);
	void SetBgColorEnd(const QColor& bg_color_end);
	void SetBarColorStart(const QColor& bar_color_start);
	void SetBarColorEnd(const QColor& bar_color_end);
	void SetCurrentValue(const QList<QVector<double> >& value);

	void RestWidget();

protected:

	void paintEvent(QPaintEvent *);
	void DrawBackground(QPainter* painter);
	void DrawBar(QPainter* painter);
	void DrawBullEyesPatternFrame(QPainter* painter);

	void DrawGradientArc(QPainter* painter, qint32 radius, qint32 startAngle,
	                     qint32 angleLength, qint32 arcHeight, double color);

private:

	void Initial();
	void Initialization();

private:

	QColor m_BgColorStart;
	QColor m_BgColorEnd;
	QColor m_BarColorStart;
	QColor m_BarColorEnd;
	QList<QVector<double> >m_CurrentValue;
	QList<QString>m_LabName;
	QComboBox* m_ComBox;

	QImage m_Gradient;
};

#endif // BULLSEYE,H_H
