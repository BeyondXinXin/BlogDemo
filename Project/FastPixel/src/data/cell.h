#ifndef CELL_H
#define CELL_H

#include <iostream>

#include <QDebug>
#include <QHash>
#include <QString>

struct Cell
{
    Cell();
    Cell(short tmp_x, short tmp_y);
    short x;
    short y;
};

// 支持 std::hash
namespace std {
template<>
struct hash<Cell>
{
    size_t operator()(const Cell& Cell) const
    {
        return hash<short>()(Cell.x) + hash<short>()(Cell.y);
    }
};
} // namespace std

// 支持 qHash
inline uint qHash(const Cell& key, uint seed)
{
    return qHash(key.x, seed) << 16 | qHash(key.y, seed);
}

// 非成员比较运算符
inline bool operator==(const Cell& lhs, const Cell& rhs)
{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

inline bool operator!=(const Cell& lhs, const Cell& rhs)
{
    return !(operator==(lhs, rhs));
}

inline Cell operator+(const Cell& lhs, const Cell& rhs)
{
    return Cell(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline Cell operator-(const Cell& lhs, const Cell& rhs)
{
    return Cell(lhs.x - rhs.x, lhs.y - rhs.y);
}

inline QDebug operator<<(QDebug dbg, const Cell& Cell)
{
    return dbg << QString("(%1,%2)").arg(Cell.x).arg(Cell.y);
}

#endif // CELL_H
