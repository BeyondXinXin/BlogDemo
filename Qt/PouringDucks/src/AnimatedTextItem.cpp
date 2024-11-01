#include "AnimatedTextItem.h"

#include <QtWidgets>

AnimatedTextItem::AnimatedTextItem(const Subtitle & subtitle, QGraphicsItem * parent)
  : QGraphicsTextItem(subtitle.text, parent)
{
    m_Subtitle = subtitle;
    setFont(m_Subtitle.font);
    setDefaultTextColor(m_Subtitle.fontColor);
    setZValue(1);
    m_EnterAnimationGroup = new QParallelAnimationGroup(this);
    m_MoveAnimationGroup = new QParallelAnimationGroup(this);
}

AnimatedTextItem::~AnimatedTextItem()
{
    if (m_EnterAnimationGroup) {
        m_EnterAnimationGroup->clear();
        m_EnterAnimationGroup->deleteLater();
        m_EnterAnimationGroup = nullptr;
    }
    if (m_MoveAnimationGroup) {
        m_MoveAnimationGroup->clear();
        m_MoveAnimationGroup->deleteLater();
        m_MoveAnimationGroup = nullptr;
    }
}

void AnimatedTextItem::SetReferenceItem(AnimatedTextItem * referenceItem, const bool & play)
{
    Subtitle refSubtitle = referenceItem->GetSubtitle();
    int delayMs = refSubtitle.startTimeMs - m_Subtitle.startTimeMs;
    bool executionPlay = play && delayMs > m_AnimationDelay;

    if (executionPlay) {
        m_ReferenceItem = referenceItem;
        PlayMoveAnimation(refSubtitle.moveAnimation);
    } else {
        MoveAnimationType type = refSubtitle.moveAnimation;
        if (type == MoveAnimationType::MoveUp) {
            setPos(pos() - QPointF(0, boundingRect().height()));
        } else if (type == MoveAnimationType::Narrow) {
            QFont tmpFont = refSubtitle.font;
            tmpFont.setPointSize(tmpFont.pointSize() - 10);
            setFont(tmpFont);
            setPos(pos() - QPointF(0, boundingRect().height()));
        } else if (type == MoveAnimationType::RotateClockwise) {
            setFont(refSubtitle.font);
            QRectF rect = boundingRect();
            QPointF pivotPoint = referenceItem->mapToScene(referenceItem->boundingRect().bottomLeft());
            setTransformOriginPoint(mapFromScene(pivotPoint));
            setRotation(rotation() - 90);
        } else if (type == MoveAnimationType::RotateCounterclockwise) {
            setFont(refSubtitle.font);
            QRectF rect = boundingRect();
            QPointF pivotPoint1 = referenceItem->mapToScene(referenceItem->boundingRect().bottomRight());
            QPointF pivotPoint2 = mapToScene(boundingRect().bottomRight());
            QPointF pivotPoint = (pivotPoint1.x() > pivotPoint2.x()) ? pivotPoint1 : pivotPoint2;
            setTransformOriginPoint(mapFromScene(pivotPoint));
            setRotation(rotation() + 90);
        }

        setParentItem(referenceItem);
    }
}

void AnimatedTextItem::ReferenceToParent()
{
    if (m_ReferenceItem != nullptr) {
        setParentItem(m_ReferenceItem);
        m_ReferenceItem = nullptr;
    }
}

QRectF AnimatedTextItem::boundingRect() const
{
    return QGraphicsTextItem::boundingRect();
}

Subtitle AnimatedTextItem::GetSubtitle()
{
    return m_Subtitle;
}

void AnimatedTextItem::TerminateAnimation()
{
    if (m_EnterAnimationGroup) {
        m_EnterAnimationGroup->stop();
        for (int i = 0; i < m_EnterAnimationGroup->animationCount(); i++) {
            QAbstractAnimation * animation = m_EnterAnimationGroup->animationAt(i);
            animation->setParent(nullptr);
            animation->deleteLater();
        }
    }
    if (m_MoveAnimationGroup) {
        m_MoveAnimationGroup->stop();
        for (int i = 0; i < m_MoveAnimationGroup->animationCount(); i++) {
            QAbstractAnimation * animation = m_MoveAnimationGroup->animationAt(i);
            animation->setParent(nullptr);
            animation->deleteLater();
        }
    }
}

void AnimatedTextItem::PlayEnterAnimation(EnterAnimationType type)
{
    TerminateAnimation();
    if (type == EnterAnimationType::FadeIn) {
        QPropertyAnimation * opacityAnim = new QPropertyAnimation(this, "opacity");
        opacityAnim->setDuration(m_AnimationDelay * 1.5);
        opacityAnim->setStartValue(0);
        opacityAnim->setEndValue(1);
        m_EnterAnimationGroup->addAnimation(opacityAnim);
    } else if (type == EnterAnimationType::ZoomIn) {
        QPropertyAnimation * scaleAnim = new QPropertyAnimation(this, "scale");
        scaleAnim->setDuration(m_AnimationDelay * 1.5);
        scaleAnim->setStartValue(.5);
        scaleAnim->setKeyValueAt(0.5, 1.1);
        scaleAnim->setEndValue(1.0);
        m_EnterAnimationGroup->addAnimation(scaleAnim);
    }
    m_EnterAnimationGroup->start(QAbstractAnimation::KeepWhenStopped);
}

void AnimatedTextItem::PlayMoveAnimation(MoveAnimationType type)
{
    TerminateAnimation();
    if (type == MoveAnimationType::MoveUp) {
        QRectF rect = boundingRect();
        QPropertyAnimation * moveUpAnim = new QPropertyAnimation(this, "pos");
        moveUpAnim->setDuration(m_AnimationDelay);
        moveUpAnim->setStartValue(pos());
        moveUpAnim->setEndValue(pos() - QPointF(0, rect.height()));
        m_MoveAnimationGroup->addAnimation(moveUpAnim);
    } else if (type == MoveAnimationType::RotateClockwise) {
        Subtitle refSubtitle = m_ReferenceItem->GetSubtitle();
        setFont(refSubtitle.font);
        QRectF rect = boundingRect();
        QPointF pivotPoint = mapToScene(boundingRect().bottomLeft());
        setTransformOriginPoint(mapFromScene(pivotPoint));
        QPropertyAnimation * rotateAnim = new QPropertyAnimation(this, "rotation");
        qreal currentRotation = rotation();
        rotateAnim->setDuration(m_AnimationDelay);
        rotateAnim->setStartValue(currentRotation);
        rotateAnim->setEndValue(currentRotation - 90);
        m_MoveAnimationGroup->addAnimation(rotateAnim);
    } else if (type == MoveAnimationType::RotateCounterclockwise) {
        Subtitle refSubtitle = m_ReferenceItem->GetSubtitle();
        setFont(refSubtitle.font);
        QRectF rect = boundingRect();
        QPointF pivotPoint1 = m_ReferenceItem->mapToScene(m_ReferenceItem->boundingRect().bottomRight());
        QPointF pivotPoint2 = mapToScene(boundingRect().bottomRight());
        QPointF pivotPoint = (pivotPoint1.x() > pivotPoint2.x()) ? pivotPoint1 : pivotPoint2;
        setTransformOriginPoint(mapFromScene(pivotPoint));
        QPropertyAnimation * rotateAnim = new QPropertyAnimation(this, "rotation");
        qreal currentRotation = rotation();
        rotateAnim->setDuration(m_AnimationDelay);
        rotateAnim->setStartValue(currentRotation);
        rotateAnim->setEndValue(currentRotation + 90);
        m_MoveAnimationGroup->addAnimation(rotateAnim);
    } else if (type == MoveAnimationType::Narrow) {
        QFont tmpFont = font();
        tmpFont.setPointSize(tmpFont.pointSize() - 10);
        setFont(tmpFont);
        QRectF rect = boundingRect();
        QPropertyAnimation * moveUpAnim = new QPropertyAnimation(this, "pos");
        moveUpAnim->setDuration(m_AnimationDelay);
        moveUpAnim->setStartValue(pos());
        moveUpAnim->setEndValue(pos() - QPointF(0, rect.height()));
        m_MoveAnimationGroup->addAnimation(moveUpAnim);
    }
    m_MoveAnimationGroup->start(QAbstractAnimation::KeepWhenStopped);
}
