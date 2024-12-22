#ifndef FrequencySpectrumWidget_H
#define FrequencySpectrumWidget_H

#include <QWidget>

class FrequencySpectrumWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FrequencySpectrumWidget(QWidget * parent = nullptr);
    void SetFrequencyData(const QList<int> & data);

protected:
    void paintEvent(QPaintEvent * event) override;
    void resizeEvent(QResizeEvent * event) override;
    void timerEvent(QTimerEvent * event) override;

private:
    struct GridCache
    {
        int gridSize;
        int gap;
        int startX;
        int startY;
    };

private:
    void PreDrawBackground();
    void UpdateFallingColors();

    void GetGridSize(int & gridSize, int & gap, int & startX, int & startY);

private:
    QList<int> m_FrequencyData;
    QVector<int> m_FallingColorHeight;
    QPixmap m_BackgroundPixmap;
    GridCache m_GridCache;
};

#endif
