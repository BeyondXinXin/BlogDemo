#ifndef STLMANAGER_H
#define STLMANAGER_H

/*
 * ModeBrowser 界面管理
*/

// 01 Frame includes
#include "stable.h"

// 03 VTK includes
#include "automaticdivision.h"
#include "customsurfaceviewer.h"
#include "customvtkrenderer.h"
#include "linesurfaceclipper.h"
#include "refinesurface.h"
#include "smoothsurfaceselector.h"
#include "vtkthreadform3dreconstruction.h"
#include "vtkthreadimagedataread.h"
#include "vtkthreadmarchingcubes.h"
#include "vtkthreadpolydataread.h"
#include "vtkthreadpolydatawrite.h"
#include "vtkthreadsubdivision.h"

// 03 CGAL includes
#include "cgalthreadfill.h"
#include "cgalthreadsubdivision.h"

// VTL includes
#include <QVTKOpenGLWidget.h>

class STLManager : public QObject
{
    Q_OBJECT
public:
    explicit STLManager(QVTKOpenGLWidget &vtk_widget,
                        QObject *parent = nullptr);
    virtual ~STLManager() override;
    void OpenModelFile(const QString &file_path,
                       vtkSmartPointer<vtkImageData> value = nullptr);
public slots:
    void SlotPolyDataHandle(const int &operation, const QString text = "");
Q_SIGNALS:
    void SingnalFinished();
    void SignalPromptInformationOut(const QString &text);
    void SignalVtkThreadProgressOut(const QString str, const int value);

private:
    void Initial();

private Q_SLOTS:
    void SlotRunFinished();
    void SlotVTKThreadMaskWidgetIn(int value = 1);
    void SlotCGLAThreadMaskWidgetIn();

private:
    QVTKOpenGLWidget &vtk_widget_; // qt窗口
    QPointer<VtkThreadPolyDataRead> reade_; // stl 读取
    QPointer<VtkThreadPolyDataWrite> write_; // stl 保存
    QList<vtkSmartPointer<vtkPolyData>> polydata_list_; // polydata数据
    QPointer<CustomVtkRenderer> renderer_; // 渲染器
    QPointer<CustomSurfaceViewer> viewer_; //模型
    QPointer<VtkThreadMarchingCubes> marching_cubes_; //imagedata转polydata
    QPointer<AutomaticDivision> auto_division_; // 提取最大连通域
    QPointer<LineSurfaceClipper> line_clipper_; // 线性剪裁
    QPointer<SmoothSurfaceSelector> smooth_selector_; // 区域平滑和修剪
    QPointer<VtkThreadSubdivision> subdivision_; // vtk细分
    QPointer<RefineSurface> refine_; // vtk细化
    QPointer<FillSurfaceSelector> cgal_selector_; // CGAL细分
    //    QPointer<CGALThreadSubdivision> cgal_subdivision_; // CGAL表面细分
    QPointer<VtkThreadForm3DReconstruction> form3dreconstruction_; // 三维重构
};

#endif // STLMANAGER_H
