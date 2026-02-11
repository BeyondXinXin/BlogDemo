#ifndef DCMMANAGER_H
#define DCMMANAGER_H

/*
 * DcmManager 界面管理
*/

// 01 Frame includes
#include "stable.h"

// 05calclation
#include "customvtkrenderer.h"
#include "vtkthreadpolydataread.h"
#include "vtkthreadimagedataread.h"
#include "vtkthreadpolydataread.h"
#include "vtkthreadmarchingcubes.h"
#include "vtkthreadpolydatawrite.h"

// VTL includes
#include <QVTKOpenGLWidget.h>
#include <vtkImagePlaneWidget.h>
#include <vtkResliceImageViewer.h>
#include <vtkGenericOpenGLRenderWindow.h>


class DcmManager : public QObject {
    Q_OBJECT
  public:
    explicit DcmManager(QVTKOpenGLWidget *dcm_widget1 = nullptr,
                        QVTKOpenGLWidget *dcm_widget2 = nullptr,
                        QVTKOpenGLWidget *dcm_widget3 = nullptr,
                        QVTKOpenGLWidget *dcm_widget = nullptr,
                        QObject *parent = nullptr);
    virtual ~DcmManager() override;
    void OpenStlFile(const QString &file_path);
  public Q_SLOTS:
    void SlotViewMaximization(const bool &value);
  Q_SIGNALS:
    void SingnalFinished();
    void SignalVtkThreadProgressOut(const QString str, const int value);
  private:
    void Initial();
  private Q_SLOTS:
    void SlotRunFinished();
    void SlotVTKThreadMaskWidgetIn(int value = 0);

  private:
    QPointer<VtkThreadImageDataRead> imagedata_read_; //imagedata读取
    QPointer<QVTKOpenGLWidget> dcm_widget1_;// qt窗口 dcm三视图
    QPointer<QVTKOpenGLWidget> dcm_widget2_;// qt窗口 dcm三视图
    QPointer<QVTKOpenGLWidget> dcm_widget3_;// qt窗口 dcm三视图
    vtkNew<vtkResliceImageViewer> riw_[3];
    vtkNew<vtkImagePlaneWidget> planeWidget_[3];
    vtkNew<vtkRenderer> renderer_;// 渲染器
    vtkNew<vtkGenericOpenGLRenderWindow> render_window_;// 窗口
    QPointer<QVTKOpenGLWidget> dcm_widget_;// qt窗口 dcm模型
};

#endif // DCMMANAGER_H
