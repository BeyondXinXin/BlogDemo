#ifndef EDITORDATA_H
#define EDITORDATA_H

#include "cell.h"

#include <QStack>

using ColorRects = QVector<QRect>;

using CellColor = QHash<Cell, QString>;
using ColorCell = QHash<QString, QList<Cell>>;

using BrushMap = QHash<QString, QBrush>;

class EditorData
{
public:
    EditorData(int num_cols, int num_rows, int cell_size);

    int Width() const; // 数据尺寸
    int Height() const; // 数据尺寸
    int Cols() const; // 数据尺寸
    int Rows() const; // 数据尺寸
    int CellSize() const; // 像素尺寸

    // 获取数据中单元格
    QList<Cell> Cells() const;
    QList<Cell> Cells(const Cell& top_left, const Cell& bottom_right) const;

    // 单元格是否被包含
    bool Contains(const Cell& node) const;

    // 单元格和坐标转换
    Cell PointToCell(const QPoint& pos) const;
    Cell PointToCell(const QPointF& pos) const;
    QPoint CellToPoint(const Cell& cell);
    QRect CellToRect(const Cell& cell) const;
    QRectF CellToRectF(const Cell& cell) const;

    // 获取图层中所有包含的颜色
    QList<QString> GetLayourColours() const;
    // 获取图层中某一个颜色对应的所有单元格rect
    ColorRects GetColourRects(const QString& name) const;

    // 获取图层中缓存数据
    CellColor GetBufferData() const;
    void ChangeBufferCell(const Cell& cells, const QString& color);
    void AddBufferData(const QList<Cell>& cells, const QString& color);
    void SetBufferData(const CellColor& data);
    void SaveBuffer();
    void SaveBufferNoAppendStack();

    void ChangePixel(const Cell& cell, const QString& color);

    QString GetCellColor(const Cell& cell) const;

    ColorCell& GetColorCell();
    QList<QString>& GetColors();

    QPixmap GetCurrentPix() const;
    QPixmap GetBufferPix() const;

    void Redo();
    void Undo();
    void AppendStack();

private:
    int num_cols_; // 列
    int num_rows_; // 行
    int cell_size_;

    QList<QString> colors_; // 网格的颜色
    CellColor buffer_cell_; // 缓冲层数据
    ColorCell color_cells_; // 颜色 - 网格

    QStack<ColorCell> undostack_color_cells_;
    QStack<QList<QString>> undostack_colors_;

    QStack<ColorCell> redostack_color_cells_;
    QStack<QList<QString>> redostack_colors_;

public:
    static QBrush GetBrush(QString& color_name);

private:
    static BrushMap colour_bruch_;
};

inline int EditorData::Width() const
{
    return num_cols_ * cell_size_;
}

inline int EditorData::Height() const
{
    return num_rows_ * cell_size_;
}

inline int EditorData::Cols() const
{
    return num_cols_;
}

inline int EditorData::Rows() const
{
    return num_rows_;
}

inline int EditorData::CellSize() const
{
    return cell_size_;
}

inline QList<QString> EditorData::GetLayourColours() const
{
    return color_cells_.keys();
}

inline QString EditorData::GetCellColor(const Cell& cell) const
{
    return colors_.at(cell.x * num_cols_ + cell.y);
}

inline ColorCell& EditorData::GetColorCell()
{
    return color_cells_;
}

inline QList<QString>& EditorData::GetColors()
{
    return colors_;
}

#endif // EDITORDATA_H
