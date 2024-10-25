#include "paint_bucket_tool.h"

#include "data/vector2d.h"
#include "editor/editor_data.h"
#include "time.h"

enum FloodFlags
{
    KFlood_Nonoe = 0,
    KFlood_In = 1,
    KFlood_Up = 2,
    KFlood_Down = 4,
};

struct Flood_Line
{
    short flags;
    short upos, dpos;
    short x;
    int next; // 链表(如果每行有多个)
};

static int flood_count = 0;
static QList<Flood_Line> flood_buf = QList<Flood_Line> {};

#define FLOOD_LINE(c) (&flood_buf[c])

static void FillLine(
  EditorData* data,
  int x,
  int top, int bottom,
  const QString& src_color, const QString& color)
{
    for (int y = top; y <= bottom; y++) {
        Cell cell(x, y);
        data->GetColors()[x * data->Cols() + y] = color;
        data->GetColorCell()[src_color].removeOne(cell);
        data->GetColorCell()[color] << cell;
    }
}

// 填充竖直条线，返回第一个处理部分的y坐标
static int Flooder(
  EditorData* data,
  int x, int y,
  const QString& src_color,
  const QString& color)
{
    Flood_Line* p;
    int up = 0, down = 0;
    int c;

    // 1 检查出发点
    if (data->GetCellColor(Cell(x, y)) != src_color) {
        return y + 1;
    }

    // 向上
    for (up = y - 1; up >= 0; up--) {
        if (data->GetCellColor(Cell(x, up)) != src_color) {
            break;
        }
    }

    // 向下
    for (down = y + 1; down < data->Rows(); down++) {
        if (data->GetCellColor(Cell(x, down)) != src_color) {
            break;
        }
    }
    up++;
    down--;

    FillLine(data, x, up, down, src_color, color);
    //    for (int new_x = up; new_x <= down; new_x++) {
    //        data->ChangePixel(Cell(new_x, y), color);
    //    }

    /* 将其存储在淹没段列表中 */
    c = x;
    p = FLOOD_LINE(c);

    if (p->flags) {
        while (p->next) {
            c = p->next;
            p = FLOOD_LINE(c);
        }

        flood_buf.reserve(flood_count);
        c = flood_count++;
        {
            Flood_Line p;
            p.flags = KFlood_Nonoe;
            p.upos = SHRT_MAX;
            p.dpos = SHRT_MIN;
            p.x = c;
            p.next = 0;
            flood_buf << p;
        }
        p = FLOOD_LINE(c);
    }

    p->flags = KFlood_In;
    p->upos = up;
    p->dpos = down;
    p->x = x;
    p->next = 0;

    if (x > 0)
        p->flags |= KFlood_Up;

    if (x + 1 < data->Rows())
        p->flags |= KFlood_Down;

    return down + 2;
}

static bool CheakFloodLine(
  EditorData* data,
  int x, int up, int down,
  const QString& src_color, const QString& color)
{

    int c;
    Flood_Line* p;
    int ret = false;

    while (up <= down) {
        c = x;

        for (;;) {
            p = FLOOD_LINE(c);

            if ((up >= p->upos) && (up <= p->dpos)) {
                up = p->dpos + 2;
                break;
            }

            c = p->next;

            if (!c) {
                up = Flooder(data, x, up, src_color, color);
                ret = true;
                break;
            } else {
                c = p->next;
            }
        }
    }

    return ret;
}

static void FloodFill(
  EditorData* data, Cell cell,
  const QString& src_color,
  const QString& color)
{

    /* 设置淹没段的列表 */
    flood_buf.clear();
    flood_count = data->Rows();
    flood_buf.reserve(flood_count);

    Flood_Line p;
    p.flags = KFlood_Nonoe;
    p.upos = SHRT_MAX;
    p.dpos = SHRT_MIN;
    p.next = 0;
    for (int c = 0; c < flood_count; c++) {
        p.x = c;
        flood_buf << p;
    }
    // 启动洪水算法
    Flooder(data, cell.x, cell.y, src_color, color);

    // 只要还有一些片段需要测试，就继续
    bool done;
    Flood_Line* line;
    do {
        done = true;

        // 对于屏幕上的每一行
        for (int c = 0; c < flood_count; c++) {

            line = FLOOD_LINE(c);

            // 右侧
            if (line->flags & KFlood_Down) {
                line->flags &= ~KFlood_Down;

                if (CheakFloodLine(data, line->x + 1, line->upos, line->dpos, src_color, color)) {
                    done = false;
                    line = FLOOD_LINE(c);
                }
            }

            // 左侧
            if (line->flags & KFlood_Up) {
                line->flags &= ~KFlood_Up;
                if (CheakFloodLine(data, line->x - 1, line->upos, line->dpos, src_color, color)) {
                    done = false;
                    // 倒退的特例捷径
                    if ((c > 0) && (data->Rows()))
                        c -= 2;
                }
            }
        }
    } while (!done);
}

PaintBucketTool::PaintBucketTool()
  : Tool()
{
    setToolTip(tr("Pen"));
    setIcon(QIcon(QPixmap(":/png/paint_bucket.png")));

    cursor_ = QCursor(QPixmap(":/png/paint_bucket.png"), -16, -16);
    real_time_update_ = true;
}

void PaintBucketTool::HandelPress(
  EditorData* data, const Cell& cell,
  const QString& color)
{
    // 判断点是否在图片上
    if (!data->Contains(cell)) {
        return;
    }

    // 要替换颜色
    auto src_color = data->GetCellColor(cell);
    if (src_color == color) { // 新颜色不能和旧颜色一样
        return;
    }

    data->AppendStack();

    FloodFill(data, cell, src_color, color);
}
