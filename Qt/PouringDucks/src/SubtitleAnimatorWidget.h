#ifndef SubtitleAnimatorWidget_H_
#define SubtitleAnimatorWidget_H_

#include <QGraphicsView>

#include "Subtitle.h"
#include "AnimatedTextItem.h"

class SubtitleAnimatorWidget : public QGraphicsView
{
    Q_OBJECT

public:
    SubtitleAnimatorWidget(QWidget * parent = nullptr);

    // 设置字幕列表
    void SetSubtitles(const QList<Subtitle> & subtitles);

    // 设置背景颜色
    void SetBackgroundColor(const QColor & color);

    // 设置当前字幕索引
    void SetCurrentIndex(const int & index);

    // 设置是否自动播放
    void SetAutoPlay(const bool & play);

    QColor GetBackgroundColor() const;

signals:

    void SgnIndexChange(int index);

    void SgnAnimationFinished();

private:
    // 显示下一个字幕
    void ShowNextSubtitle();

    // 播放字幕动画
    void DisplaySubtitle(const Subtitle & subtitle);

    // 启动屏幕震动效果
    void StartScreenShake(int durationMs, int amplitude);

private:
    QGraphicsScene m_Scene;
    QList<Subtitle> m_Subtitles;
    QList<AnimatedTextItem *> m_DisplayedItems;
    size_t m_CurrentIndex = 0;
    bool m_Playing = false;

    AnimatedTextItem * m_OriItem;
};

#endif // ifndef SoundManager_H_
