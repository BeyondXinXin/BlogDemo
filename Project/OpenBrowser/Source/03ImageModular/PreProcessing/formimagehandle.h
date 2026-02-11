#ifndef FORMIMAGEHANDLE_H
#define FORMIMAGEHANDLE_H

/*
 * ImageBrowser 菜单栏
 */

// 01 Frame includes
#include "stable.h"

namespace Ui {
    class FormImageHandle;
}

class FormImageHandle : public QWidget {
    Q_OBJECT

  public:
    explicit FormImageHandle(QWidget *parent = nullptr);
    ~FormImageHandle();
    void ShowImageHandel(const int num);

  Q_SIGNALS:
    void SingalImageMenuOut(const int &operation, const QString &text = "");
    void SignalPromptInformationOut(const QString &text);
  private:
    void WinGeom();								// 几何变换窗口 101
    void WinGray();								// 灰度变换窗口 201
    void WinEnhance();							// 图像增强窗口 301
    void WinMorp();								// 形态学处理窗口 401
    void WinColor();							// 颜色模型窗口 501
    void WinTest();							    // 测试窗口 601
  private:
    Ui::FormImageHandle *ui;

    QWidget *dock_Geom;
    QWidget *dock_Gray;
    QWidget *dock_Enhance;
    QWidget *dock_Morp;
    QWidget *dock_Color;
    QWidget *dock_Test;
};

#endif // FORMIMAGEHANDLE_H
