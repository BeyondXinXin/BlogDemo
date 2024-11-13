#include "SubtitleAnimatorWidget.h"

#include <QtWidgets>

SubtitleAnimatorWidget::SubtitleAnimatorWidget(QWidget * parent)
  : QGraphicsView(parent)
{
    setScene(&m_Scene);
    setRenderHint(QPainter::Antialiasing);
    int resolution = 60;
    setFixedSize(16 * resolution, 9 * resolution);
    m_Scene.setSceneRect(0, 0, 16 * (resolution + 2), 9 * (resolution + 2));

    Subtitle oriSbutitle;
    m_OriItem = new AnimatedTextItem(oriSbutitle);

    m_Scene.addItem(m_OriItem);
    QPointF initialPos(6 * resolution, m_Scene.height() / 2);
    m_OriItem->setPos(initialPos);

    centerOn(325, 325);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    SetBackgroundColor(QColor(5, 5, 5));
    setStyleSheet(QString("QGraphicsView { padding: -5;}"));
}

void SubtitleAnimatorWidget::SetSubtitles(const QList<Subtitle> & subtitles)
{
    int maxCount = 400;
    int count = qMin(maxCount, subtitles.size());
    m_Subtitles = std::move(subtitles.mid(0, count));
    m_CurrentIndex = 0;
    SetCurrentIndex(m_CurrentIndex);
}

void SubtitleAnimatorWidget::SetBackgroundColor(const QColor & color)
{
    setBackgroundBrush(QBrush(color));
}

QColor SubtitleAnimatorWidget::GetBackgroundColor() const
{
    return backgroundBrush().color();
}

void SubtitleAnimatorWidget::SetCurrentIndex(const int & index)
{
    if (m_Playing) {
        return;
    }
    foreach (auto * item, m_DisplayedItems) {
        item->TerminateAnimation();
        item->setParent(nullptr);
        m_Scene.removeItem(item);
        item->deleteLater();
    }
    m_DisplayedItems.clear();

    m_CurrentIndex = index;
    if (m_CurrentIndex > m_Subtitles.size()) {
        m_CurrentIndex = 0;
    }

    for (int i = 0; i < m_CurrentIndex; i++) {
        Subtitle subtitle = m_Subtitles.at(i);
        AnimatedTextItem * proxyItem = new AnimatedTextItem(subtitle, m_OriItem);
        if (m_DisplayedItems.size() > 0) {
            m_DisplayedItems.last()->SetReferenceItem(proxyItem);
        }
        m_DisplayedItems.push_back(proxyItem);
    }
}

void SubtitleAnimatorWidget::SetAutoPlay(const bool & play)
{
    m_Playing = play;
    if (m_Playing) {
        ShowNextSubtitle();
    }
}

void SubtitleAnimatorWidget::ShowNextSubtitle()
{
    if (m_CurrentIndex >= m_Subtitles.size() && m_Subtitles.size() > 1) {
        m_Playing = false;
        int delayMs = m_Subtitles.last().durationMs;
        QTimer::singleShot(delayMs, this, [&]() {
            emit SgnAnimationFinished();
        });
    }
    if (m_Playing) {
        const auto & subtitle = m_Subtitles[m_CurrentIndex];
        int delayMs = subtitle.startTimeMs - (m_CurrentIndex == 0 ? 0 : m_Subtitles[m_CurrentIndex - 1].startTimeMs);
        if (m_CurrentIndex == m_Subtitles.size() - 1) {
            delayMs = subtitle.durationMs;
        }
        QTimer::singleShot(delayMs, this, [this, subtitle]() {
            if (m_DisplayedItems.size() > 1) {
                m_DisplayedItems[m_DisplayedItems.size() - 2]->ReferenceToParent();
            }
            if (m_Playing) {
                emit SgnIndexChange(m_CurrentIndex);
            }
            DisplaySubtitle(subtitle);
        });
        m_CurrentIndex++;
    }
}

void SubtitleAnimatorWidget::DisplaySubtitle(const Subtitle & subtitle)
{
    AnimatedTextItem * proxyItem = new AnimatedTextItem(subtitle, m_OriItem);
    if (m_DisplayedItems.size() > 0) {
        m_DisplayedItems.last()->SetReferenceItem(proxyItem, true);
    }
    proxyItem->PlayEnterAnimation(subtitle.enterAnimation);
    // StartScreenShake(100, 10);
    m_DisplayedItems.push_back(proxyItem);
    ShowNextSubtitle();
}

void SubtitleAnimatorWidget::StartScreenShake(int durationMs, int amplitude)
{
    QTimer * timer = new QTimer(this);
    const QPointF originalPos = this->pos();
    bool isShaken = false;

    connect(timer, &QTimer::timeout, this, [=]() mutable {
        if (isShaken) {
            this->move(originalPos.toPoint());
            timer->stop();
            timer->deleteLater();
        } else {
            int offsetX = QRandomGenerator::global()->bounded(-amplitude, amplitude + 1);
            int offsetY = QRandomGenerator::global()->bounded(-amplitude, amplitude + 1);
            this->move(originalPos.x() + offsetX, originalPos.y() + offsetY);
            isShaken = true;
        }
    });

    timer->start(durationMs / 2);
}