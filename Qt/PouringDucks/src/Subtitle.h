#ifndef Subtitle_H
#define Subtitle_H

#include <QString>
#include <QColor>
#include <QFont>

enum class EnterAnimationType {
    FadeIn,
    ZoomIn
};

enum class MoveAnimationType {
    MoveUp,
    RotateClockwise,
    RotateCounterclockwise,
    Narrow
};

struct Subtitle {
    QString text;
    qint64 startTimeMs;
    qint64 durationMs;
    QFont font;
    QColor fontColor = Qt::darkGray;
    EnterAnimationType enterAnimation;
    MoveAnimationType moveAnimation;
};

#endif  // Subtitle_H
