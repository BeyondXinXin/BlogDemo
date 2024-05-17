#include "BullsEye.h"

#include <QPainter>
#include <QPainterPath>

BullsEye::BullsEye(QWidget* parent) :
	QWidget(parent)
{
	this->Initial();
	this->Initialization();
}

BullsEye::~BullsEye()
{}

QColor BullsEye::GetBgColorStart() const
{
	return this->m_BgColorStart;
}

QColor BullsEye::GetBgColorEnd() const
{
	return this->m_BgColorEnd;
}

QColor BullsEye::GetBarColorStart() const
{
	return this->m_BarColorStart;
}

QColor BullsEye::GetBarColorEnd() const
{
	return this->m_BarColorEnd;
}

QList<QVector<double> >BullsEye::GetCurrentValue() const
{
	return this->m_CurrentValue;
}

QRgb BullsEye::GetGradient(const double value)
{
	/*!
	   @details   根据牛眼图中每块的数值（0-1），去色带查找具体颜色
	 */
	QRgb color = this->m_Gradient.pixel(8, 310 - static_cast<qint32>(value * 310));
	return color;
}

void BullsEye::SetBgColorStart(const QColor& bg_color_start)
{
	this->m_BgColorStart = bg_color_start;
}

void BullsEye::SetBgColorEnd(const QColor& bg_color_end)
{
	this->m_BgColorEnd = bg_color_end;
}

void BullsEye::SetBarColorStart(const QColor& bar_color_start)
{
	this->m_BarColorStart = bar_color_start;
}

void BullsEye::SetBarColorEnd(const QColor& bar_color_end)
{
	this->m_BarColorEnd = bar_color_end;
}

void BullsEye::SetCurrentValue(const QList<QVector<double> >& value)
{
	this->m_CurrentValue.clear();
	this->m_CurrentValue << value;
	this->update();
}

/**
 * @brief BullsEye::RestWidget
 * 界面清空
 */
void BullsEye::RestWidget()
{
	QVector<double> value;
	value << 0 << 0 << 0 << 0 << 0 << 0
	      << 0 << 0 << 0 << 0 << 0 << 0
	      << 0 << 0 << 0 << 0 << 0;
	this->m_CurrentValue.clear();
	this->m_CurrentValue << value << value << value;
	this->update();
}

void BullsEye::paintEvent(QPaintEvent *)
{
	QPainter painter(this);


	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	this->DrawBackground(&painter);
	this->DrawBar(&painter);
	this->DrawBullEyesPatternFrame(&painter);
	this->m_ComBox->move(this->width() - m_ComBox->width() - 20, 20);
}

void BullsEye::DrawBackground(QPainter* painter)
{
	painter->save();
	painter->setPen(Qt::NoPen);
	QLinearGradient bgGradient(QPointF(0, 0), QPointF(0, height()));
	bgGradient.setColorAt(0.0, m_BgColorStart);
	bgGradient.setColorAt(1.0, m_BgColorEnd);
	painter->setBrush(bgGradient);
	painter->drawRect(rect());
	painter->restore();
}

void BullsEye::DrawBar(QPainter* painter)
{
	/*!
	   @details   根据widget宽计算色带大小
	 */
	painter->save();
	painter->setPen(Qt::NoPen);
	QLinearGradient color_gradient(QPointF(width() / 8, height() / 10),
	                               QPointF(width() / 8, 9 * height() / 10));
	QRectF bar_rect(QPointF(width() / 8, height() / 10),
	                QPointF(20 + width() / 8, 9 * height() / 10));
	color_gradient.setColorAt(0.0, qRgb(255, 0, 0));
	color_gradient.setColorAt(1.0, qRgb(0, 0, 0));
	painter->setBrush(color_gradient);
	painter->drawRect(bar_rect);

	/*!
	   @details   根据widget宽计算文字大小和位置
	 */
	QFont f(font());
	f.setPixelSize(static_cast<qint32>((width() / 10) * 0.25));
	painter->setFont(f);
	painter->setPen(Qt::white);
	painter->drawText(width() / 18, 9 * height() / 10,      "    0%");
	painter->drawText(width() / 18, 5 * height() / 10,      "   50%");
	painter->drawText(width() / 18, 1 * height() / 10 + 12, "100%");
	painter->drawText(-f.pixelSize() * 30 + 18 * width() / 20,
	                  19 * height() / 20 + 10,
	                  m_ComBox->currentText());
	painter->restore();
}

void BullsEye::DrawBullEyesPatternFrame(QPainter* painter)
{
	painter->save();

	/*!
	   @details   根据widget宽计算牛眼图尺寸
	 */
	qint32 arc_height = width() / 16;
	qint32 radius_4 = 4 * arc_height;
	qint32 radius_3 = 3 * arc_height;
	qint32 radius_2 = 2 * arc_height;
	qint32 radius_1 = arc_height;

	/*!
	   @details 设计圆心以及画牛眼图
	 */
	QVector<double> value = m_CurrentValue.at(m_ComBox->currentIndex());
	painter->translate((width() >> 1) * 1.2, height() >> 1);
	DrawGradientArc(painter, radius_1, 0,   360, arc_height, value[16]);
	DrawGradientArc(painter, radius_2, 45,  90,  arc_height, value[12]);
	DrawGradientArc(painter, radius_2, 135, 90,  arc_height, value[13]);
	DrawGradientArc(painter, radius_2, 225, 90,  arc_height, value[14]);
	DrawGradientArc(painter, radius_2, 315, 90,  arc_height, value[15]);
	DrawGradientArc(painter, radius_3, 0,   60,  arc_height, value[6]);
	DrawGradientArc(painter, radius_3, 60,  60,  arc_height, value[7]);
	DrawGradientArc(painter, radius_3, 120, 60,  arc_height, value[8]);
	DrawGradientArc(painter, radius_3, 180, 60,  arc_height, value[9]);
	DrawGradientArc(painter, radius_3, 240, 60,  arc_height, value[10]);
	DrawGradientArc(painter, radius_3, 300, 60,  arc_height, value[11]);
	DrawGradientArc(painter, radius_4, 0,   60,  arc_height, value[0]);
	DrawGradientArc(painter, radius_4, 60,  60,  arc_height, value[1]);
	DrawGradientArc(painter, radius_4, 120, 60,  arc_height, value[2]);
	DrawGradientArc(painter, radius_4, 180, 60,  arc_height, value[3]);
	DrawGradientArc(painter, radius_4, 240, 60,  arc_height, value[4]);
	DrawGradientArc(painter, radius_4, 300, 60,  arc_height, value[5]);
	painter->restore();
}

void BullsEye::DrawGradientArc(QPainter* painter,
                               qint32 radius,
                               qint32 startAngle,
                               qint32 angleLength,
                               qint32 arcHeight,
                               double color)
{
	color = QString::number(color, 'f', 2).toDouble();

	/*!
	   @details 梯形环计算
	 */
	painter->save();
	QRadialGradient gradient(0, 0, radius);

	// gradient.setColorAt(0, Qt::white);
	gradient.setColorAt(0, GetGradient(color));
	gradient.setColorAt(1, GetGradient(color));
	painter->setBrush(gradient);
	QRectF rect(-radius, -radius, radius << 1, radius << 1);
	QPainterPath path;
	path.arcTo(rect, startAngle, angleLength);
	QPainterPath subPath;
	subPath.addEllipse(rect.adjusted(arcHeight, arcHeight, -arcHeight, -arcHeight));
	path -= subPath;
	painter->setPen(Qt::NoPen);
	painter->drawPath(path);
	QFont f(font());
	f.setPixelSize(static_cast<qint32>((width() / 10) * 0.2));
	painter->setFont(f);
	painter->setPen(Qt::white);
	double radius_text = radius - arcHeight / 2;
	double angle_text = (startAngle + angleLength / 2) / 180.0 * 3.14159265;
	double center_text_x = radius_text * cos(angle_text) - f.pixelSize() * 1;
	double center_text_y = -radius_text * sin(angle_text) - f.pixelSize() * 0.5;
	painter->drawText(static_cast<qint32>(center_text_x),
	                  static_cast<qint32>(center_text_y),
	                  f.pixelSize() * 2, f.pixelSize(),
	                  Qt::AlignVCenter | Qt::AlignHCenter,
	                  QString::number(color));
	painter->restore();
}

void BullsEye::Initial()
{
	this->m_BgColorStart = QColor(55, 52, 56);
	this->m_BgColorEnd = QColor(55, 52, 56);
	this->m_BarColorStart = QColor(255, 0, 0);
	this->m_BarColorEnd = QColor(0, 0, 255);
	QVector<double> value;
	value << 0.06 << 0.12 << 0.18 << 0.24 << 0.29 << 0.35
	      << 0.41 << 0.47 << 0.53 << 0.59 << 0.65 << 0.71
	      << 0.76 << 0.82 << 0.88 << 0.94 << 1.00;
	this->m_CurrentValue << value << value << value;
	this->m_Gradient.load(":/new/prefix1/gradient.png");
	this->m_LabName << u8"  壁厚百分比  " << u8"  內径百分比  " << u8"  体积百分比  ";
	this->m_ComBox = new QComboBox(this);
	this->m_ComBox->addItems(m_LabName);
}

void BullsEye::Initialization()
{
	connect(m_ComBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
	        this, [&](){
		this->update();
	});
}
