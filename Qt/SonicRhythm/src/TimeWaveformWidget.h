#ifndef TimeWaveformWidget_H
#define TimeWaveformWidget_H

#include <QWidget>

class QBarSet;
class QStackedBarSeries;
class QBarCategoryAxis;
class QValueAxis;
class QChart;
class QChartView;
class QSplineSeries;

class TimeWaveformWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TimeWaveformWidget(QWidget * parent = nullptr);

    void SetSeriesData(const QList<int> & freqData, const QList<int> & timeData);

protected:
    void timerEvent(QTimerEvent * event) override;

private:
    void InitFreqSeries();
    void InitTimeSeries();

    float CatmullRomInterpolate(
      float p0, float p1, float p2, float p3, float t);
    void InterpolateData(
      const QVector<QPointF> & originalPoints,
      QVector<QPointF> & targetPoints, int targetCount);

private:
    QChart * m_Chart = nullptr;
    QChartView * m_ChartView = nullptr;

    QBarSet * m_FreqBarSet = nullptr;
    QBarCategoryAxis * m_FreqAxisX = nullptr;
    QStackedBarSeries * m_FreqSeries = nullptr;
    QValueAxis * m_FreqAxisY = nullptr;
    QSplineSeries * m_TimeSeries = nullptr;
    QBarSet * m_TimeBarSet = nullptr;

    QList<int> m_FreqData;
    QList<int> m_TimeData;
};

#endif
