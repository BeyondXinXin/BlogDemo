#include "TimeWaveformWidget.h"

#include <QtWidgets>
#include <QChart>
#include <QBarSet>
#include <QStackedBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QSplineSeries>
#include <QChartView>

constexpr int FreqAxisRange = 512;
constexpr int TimeAxisRange = 1024;
constexpr int FreqDomainSum = 20;

constexpr int FPS = 30;

TimeWaveformWidget::TimeWaveformWidget(QWidget * parent)
  : QWidget(parent)
  , m_FreqBarSet(new QBarSet(""))
  , m_FreqSeries(new QStackedBarSeries)
  , m_Chart(new QChart)
  , m_FreqAxisX(new QBarCategoryAxis)
  , m_FreqAxisY(new QValueAxis)
  , m_TimeSeries(new QSplineSeries)
  , m_TimeBarSet(new QBarSet(""))
{
    m_ChartView = new QChartView(m_Chart, this);
    m_ChartView->setRenderHint(QPainter::Antialiasing);
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addWidget(m_ChartView);
    setLayout(layout);

    InitFreqSeries();
    InitTimeSeries();

    m_Chart->legend()->setVisible(false);
    m_Chart->legend()->setAlignment(Qt::AlignBottom);
    m_Chart->setTheme(QChart::ChartThemeDark);
    m_Chart->setAnimationOptions(QChart::NoAnimation);

    startTimer(FPS);
}

void TimeWaveformWidget::SetSeriesData(const QList<int> & freqData, const QList<int> & timeData)
{
    m_FreqData = std::move(freqData);
    m_TimeData = std::move(timeData);
}

void TimeWaveformWidget::timerEvent(QTimerEvent * event)
{
    // 更新频域数据
    if (m_FreqData.size() == FreqDomainSum) {
        for (int i = 0; i < m_FreqData.size(); ++i) {
            m_FreqBarSet->replace(i, m_FreqData.at(i));
        }
    }

    // 更新时域数据
    QVector<QPointF> points;
    int sampleCount = m_TimeData.size();
    QVector<QPointF> originalPoints;
    for (int i = 0; i < sampleCount; ++i) {
        originalPoints.emplace_back(i, m_TimeData.at(i));
    }
    InterpolateData(originalPoints, points, TimeAxisRange);
    m_TimeSeries->replace(points);

    m_Chart->update();
}

void TimeWaveformWidget::InitFreqSeries()
{
    for (int i = 0; i < FreqDomainSum; i++) {
        *m_FreqBarSet << 0;
    }

    m_FreqSeries->append(m_FreqBarSet);
    m_Chart->addSeries(m_FreqSeries);
    m_Chart->addAxis(m_FreqAxisX, Qt::AlignBottom);
    m_FreqAxisY->setRange(0, FreqAxisRange);
    m_Chart->addAxis(m_FreqAxisY, Qt::AlignLeft);
    m_FreqSeries->attachAxis(m_FreqAxisX);
    m_FreqSeries->attachAxis(m_FreqAxisY);

    m_FreqAxisX->setLineVisible(false);
    m_FreqAxisX->setLabelsVisible(false);
    m_FreqAxisX->setGridLineVisible(false);
    m_FreqAxisY->setLineVisible(false);
    m_FreqAxisY->setLabelsVisible(false);
    m_FreqAxisY->setGridLineVisible(false);
}

void TimeWaveformWidget::InitTimeSeries()
{
    m_Chart->addSeries(m_TimeSeries);

    auto axisX = new QValueAxis;
    axisX->setRange(0, TimeAxisRange);
    auto axisY = new QValueAxis;
    axisY->setRange(0, 256);
    m_Chart->addAxis(axisX, Qt::AlignBottom);
    m_TimeSeries->attachAxis(axisX);
    m_Chart->addAxis(axisY, Qt::AlignLeft);
    m_TimeSeries->attachAxis(axisY);

    axisX->setLineVisible(false);
    axisX->setLabelsVisible(false);
    axisX->setGridLineVisible(false);
    axisY->setLineVisible(false);
    axisY->setLabelsVisible(false);
    axisY->setGridLineVisible(false);
}

float TimeWaveformWidget::CatmullRomInterpolate(
  float p0, float p1, float p2, float p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;
    float result = 0.5f * ((2.0f * p1) + (-p0 + p2) * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 + (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
    return result;
}

void TimeWaveformWidget::InterpolateData(
  const QVector<QPointF> & originalPoints,
  QVector<QPointF> & targetPoints, int targetCount)
{
    int n = originalPoints.size();
    if (n < 2) {
        return;
    }

    targetPoints.reserve(targetCount);
    float step = static_cast<float>(n - 1) / (targetCount - 1);
    for (int i = 0; i < targetCount; ++i) {
        float t = i * step;
        int index = static_cast<int>(t);
        float frac = t - index;

        int p0 = (index > 0) ? (index - 1) : 0;
        int p1 = index;
        int p2 = (index + 1 < n) ? (index + 1) : (n - 1);
        int p3 = (index + 2 < n) ? (index + 2) : (n - 1);

        float value = CatmullRomInterpolate(
          originalPoints[p0].y(), originalPoints[p1].y(),
          originalPoints[p2].y(), originalPoints[p3].y(), frac);

        targetPoints.append(QPointF(i, value));
    }
}
