#ifndef FORMEDGEADJUSTMENT_H
#define FORMEDGEADJUSTMENT_H

#include "stable.h"
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <QVTKOpenGLWidget.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkSphereSource.h>

namespace Ui {
    class FormEdgeAdjustment;
}

class FormEdgeAdjustment : public QWidget {
    Q_OBJECT
  public:
    explicit FormEdgeAdjustment(QWidget *parent = nullptr);
    ~FormEdgeAdjustment();
  private slots:
    void SlotAdjustClickBegin();// 边缘调整控件 左键按下事件
    void SlotAdjustClickEnd();// 边缘调整控件 左键松开事件
    void SlotAdjustMove();// 边缘调整控件 左键移动事件
    void SlotAdjustKeyPress();// 边缘调整控件 键盘按下事件
    void SlotAdjustKeyRelese();// 边缘调整控件 键盘松开事件
  private:
    void filtering(QList<QList<double>> &data);
    void splinefiltering(QList<QList<double>> &data, qint32 begin, qint32 end);
    void SetCursorChange();// 边缘调整控件 光标调整事件
    void SetAdjustWidget(const bool adjust_l, const bool adjust_r);
  private:
    Ui::FormEdgeAdjustment *ui;
    vtkSmartPointer<vtkRenderer> select_render_;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> render_window_;
    vtkSmartPointer<vtkEventQtSlotConnect> vtk_connections_;
  private:
    QList<vtkSmartPointer<vtkActor>> adjust_actor_list_;// 边缘调整控件 显示Actor
    vtkSmartPointer<vtkPoints> points_;// 边缘调整控件 点数据
    QList<int> edge_chinge_id_;// 边缘调整控件 每次调整点序号
    vtkNew<vtkSphereSource> sphereSource ;// 边缘调整控件 点级
    qint32 line_adjustment_size_;// 边缘调整控件 鼠标移动选择点范围
    qint32 number_adjust_left_;// 边缘调整控件 左边界点数量
    qint32 number_adjust_regit_;// 边缘调整控件 右边界点数量
    qint32 number_currentl_left_;// 边缘调整控件 左边界选中点编号
    qint32 number_currentl_regit_;// 边缘调整控件 右边界选中点编号
    vtkNew<vtkPolyData> polydata_adjust_;// 边缘调整控件 数据级
    QList<bool> current_adjust_bool_;// 边缘调整控件 当前是否移动 是否平滑
    QList<double> adjust_click_;// 边缘调整控件 鼠标点击记录点
};

#endif // FORMEDGEADJUSTMENT_H
