#include "FrequencySpectrumWidget.h"

#include <QBarCategoryAxis>
#include <QBarSeries>
#include <QBarSet>
#include <QChart>
#include <QLegend>
#include <QStackedBarSeries>
#include <QValueAxis>
#include <QChartView>
#include <QVBoxLayout>
#include <QLineSeries>

constexpr int Rows = 12;
constexpr int Cols = 20;
constexpr int GridGap = 4;
constexpr int Falling = 50;
constexpr bool EnableAntialiasing = true;

constexpr int MaxFrequency = 512; // 频率数据最大值
// 色相范围
constexpr int MaxHue = 120;
constexpr int MaxSaturation = 180;
constexpr int MaxValue = 180;

FrequencySpectrumWidget::FrequencySpectrumWidget(QWidget * parent)
  : QWidget(parent)
  , m_FrequencyData(Cols, 0)
  , m_FallingColorHeight(Cols, 1)
{
    m_BackgroundPixmap = QPixmap(width(), height());
    PreDrawBackground();
    startTimer(Falling);
}

void FrequencySpectrumWidget::SetFrequencyData(const QList<int> & data)
{
    if (data.size() != Cols) {
        return;
    }

    for (int col = 0; col < Cols; ++col) {
        int sound = data[col] * Rows / MaxFrequency;
        sound = qMin(sound, Rows);
        m_FrequencyData[col] = sound;
        m_FallingColorHeight[col] = qMax(sound, m_FallingColorHeight[col]);
    }
}

void FrequencySpectrumWidget::resizeEvent(QResizeEvent * event)
{
    QWidget::resizeEvent(event);

    if (m_BackgroundPixmap.size() != QSize(width(), height())) {
        GetGridSize(m_GridCache.gridSize, m_GridCache.gap, m_GridCache.startX, m_GridCache.startY);
        PreDrawBackground();
    }
}

void FrequencySpectrumWidget::timerEvent(QTimerEvent * event)
{
    UpdateFallingColors();
    update();
}

void FrequencySpectrumWidget::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, EnableAntialiasing);

    // 绘制缓存的背景图像
    painter.drawPixmap(0, 0, m_BackgroundPixmap);

    const int gridSize = m_GridCache.gridSize;
    const int gap = m_GridCache.gap;
    const int startX = m_GridCache.startX;
    const int startY = m_GridCache.startY;

    // 绘制频域数据相关的格子
    for (int col = 0; col < Cols; ++col) {
        int volumeLevel = m_FrequencyData[col];
        for (int i = 0; i < volumeLevel; ++i) {
            int row = Rows - 1 - i;
            int hue = MaxHue - (i * MaxHue / Rows);
            int saturation = MaxSaturation;
            int value = MaxValue;
            QColor color = QColor::fromHsv(hue, saturation, value);
            QRect rect(startX + col * (gridSize + gap),
                       startY + row * (gridSize + gap),
                       gridSize, gridSize);
            painter.setBrush(QBrush(color));
            painter.drawRect(rect);
        }

        int heightRow = Rows - 1 - m_FallingColorHeight[col];
        QColor color(200, 200, 200);
        QRect rect(
          startX + col * (gridSize + gap),
          startY + heightRow * (gridSize + gap) + gridSize / 2,
          gridSize, gridSize / 2);
        painter.setBrush(QBrush(color));
        painter.drawRect(rect);
    }
}

void FrequencySpectrumWidget::PreDrawBackground()
{
    m_BackgroundPixmap = QPixmap(width(), height());
    m_BackgroundPixmap.fill(Qt::black); // 填充背景为黑色

    QPainter painter(&m_BackgroundPixmap);
    painter.setRenderHint(QPainter::Antialiasing, EnableAntialiasing);

    const int gridSize = m_GridCache.gridSize;
    const int gap = m_GridCache.gap;
    const int startX = m_GridCache.startX;
    const int startY = m_GridCache.startY;

    QVector<QRect> gridRects;

    for (int col = 0; col < Cols; ++col) {
        for (int row = 0; row < Rows; ++row) {
            gridRects.append(QRect(
              startX + col * (gridSize + gap),
              startY + row * (gridSize + gap),
              gridSize, gridSize));
        }
    }

    QColor color(22, 23, 21);
    painter.setBrush(QBrush(color));
    painter.drawRects(gridRects.data(), gridRects.size());
}

void FrequencySpectrumWidget::UpdateFallingColors()
{
    for (int col = 0; col < Cols; ++col) {
        // m_FallingColorHeight[col] = qMax(m_FrequencyData[col], m_FallingColorHeight[col] - 1);
        m_FallingColorHeight[col] = qMax(
          m_FrequencyData[col],
          m_FallingColorHeight[col] - qCeil((m_FallingColorHeight[col] - m_FrequencyData[col]) * 0.1));
    }
}

void FrequencySpectrumWidget::GetGridSize(int & gridSize, int & gap, int & startX, int & startY)
{
    gap = GridGap; // 网格间隙（1:1比例）

    int availableWidth = width() - (Cols + 1) * gap; // 可用宽度（减去左右间隙）
    int availableHeight = height() - (Rows + 1) * gap; // 可用高度（减去上下间隙）
    gridSize = std::min(availableWidth / Cols, availableHeight / Rows); // 格子边长
    if (gridSize < gap * 2) { // 尺寸太小，保证至少有两个格子宽度的间隙
        gridSize = gap * 2;
    }

    // 计算总的网格宽度和高度
    int totalWidth = (Cols * gridSize) + (Cols + 1) * gap;
    int totalHeight = (Rows * gridSize) + (Rows + 1) * gap;

    // 计算绘制起始位置，使网格居中
    startX = (width() - totalWidth) / 2;
    startY = (height() - totalHeight) / 2;
}
