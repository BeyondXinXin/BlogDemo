#ifndef SCREENWIDGET_H
#define SCREENWIDGET_H

/**
 * 等待进度条控件
 * 1:圆弧状风格 旋转圆风格 三角圆弧 线条风格 圆环风格
 * 2:可设置范围值和当前值
 * 3:可设置前景色背景色
 * 4:可设置顺时针逆时针旋转
 * 5:支持任意大小缩放
 * 6:支持设置旋转速度间隔
 */

// 01 Frame includes
#include "stable.h"

class Screen {
  public:
    enum STATUS {SELECT, MOV, SET_W_H};
    Screen() {}
    Screen(QSize size);

    void setStart(QPoint pos);
    void setEnd(QPoint pos);
    QPoint getStart();
    QPoint getEnd();

    QPoint getLeftUp();
    QPoint getRightDown();

    STATUS getStatus();
    void setStatus(STATUS status);

    int width();
    int height();
    bool isInArea(QPoint pos);          // 检测pos是否在截图区域内
    void move(QPoint p);                // 按 p 移动截图区域

  private:
    QPoint leftUpPos, rightDownPos;     //记录 截图区域 左上角、右下角
    QPoint startPos, endPos;            //记录 鼠标开始位置、结束位置
    int maxWidth, maxHeight;            //记录屏幕大小
    STATUS status;                      //三个状态: 选择区域、移动区域、设置width height

    void cmpPoint(QPoint &s, QPoint &e);//比较两位置，判断左上角、右下角
};

//全局截屏控件、鼠标右键选择菜单|鼠标右键选择菜单|图片另存为//
class ScreenWidget : public QWidget {
    Q_OBJECT
  public:
    static ScreenWidget *Instance();
    explicit ScreenWidget(QWidget *parent = nullptr);
    ~ScreenWidget();

  private:
    static ScreenWidget *instance;
    QMenu *menu;            //右键菜单对象
    Screen *screen;         //截屏对象
    QPixmap *fullScreen;    //保存全屏图像
    QPixmap *bgScreen;      //模糊背景图
    QPoint movPos;          //坐标

  protected:
    void contextMenuEvent(QContextMenuEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
    void showEvent(QShowEvent *);

  private slots:
    void initForm();
    void saveScreen();
    void saveScreenOther();
    void saveFullScreen();
    void exitScreen();
};


#endif // SCREENWIDGET_H
