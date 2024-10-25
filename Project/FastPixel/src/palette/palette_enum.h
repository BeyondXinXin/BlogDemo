#ifndef PALETTEENUM_H
#define PALETTEENUM_H

#include <QList>
#include <QColor>

/**
 * @brief The ColorSpec enum
 * 软件使用的颜色类型,目前仅支持RBG模式
 */
enum ColorSpec
{
    K_CS_RGB = 0,
    K_CS_ARGB = 1
};

/**
 * @brief The ColorType enum
 * 颜色类型
 * 主色
 * 负色
 * 空
 */
enum ColorType
{
    K_CT_None = 0,
    K_CT_Primary = 1,
    K_CT_Secondary = 2,
};

using Colors = QList<QColor>;

#endif // PALETTEENUM_H
