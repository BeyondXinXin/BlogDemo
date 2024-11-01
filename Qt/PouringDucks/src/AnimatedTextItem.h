#ifndef AnimatedTextItem_H_
#define AnimatedTextItem_H_

#include <QGraphicsTextItem>

#include "Subtitle.h"

class QParallelAnimationGroup;

/*
    Graphics 框架下一个带动画效果的文本项
*/
class AnimatedTextItem : public QGraphicsTextItem
{
    Q_OBJECT

public:
    AnimatedTextItem(const Subtitle & subtitle, QGraphicsItem * parent = nullptr);
    ~AnimatedTextItem();

    QRectF boundingRect() const override;

    Subtitle GetSubtitle();

    // 设置参考项(上一个动画)，不播放就直接设置位置，播放就生成运动动画
    void SetReferenceItem(AnimatedTextItem * referenceItem, const bool & play = false);
    // 将参考项设置为父类
    void ReferenceToParent();

    // 播放、终止动画
    void TerminateAnimation();
    void PlayEnterAnimation(EnterAnimationType type);
    void PlayMoveAnimation(MoveAnimationType type);

private:
    AnimatedTextItem * m_ReferenceItem = nullptr;
    QParallelAnimationGroup * m_EnterAnimationGroup = nullptr;
    QParallelAnimationGroup * m_MoveAnimationGroup = nullptr;
    Subtitle m_Subtitle;
    int m_AnimationDelay = 200;
};

#endif // ifndef SoundManager_H_
