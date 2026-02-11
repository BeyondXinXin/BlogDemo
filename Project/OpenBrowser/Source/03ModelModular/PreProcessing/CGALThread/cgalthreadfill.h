#ifndef CGALTHREADFILL_H
#define CGALTHREADFILL_H

/*
 * CGAL + VTK
 * 表面补洞
 * vtk交互寻找需要补的洞（预览功能）
 * CGAL负责补洞，如果失败则返回VTK处理结果
 * 未写进线程，界面会出现假死状态
 * */

// 01 Frame includes
#include "stable.h"

// 03 VTK includes
#include "customvtkrenderer.h"

// VTK includes
#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkSeedWidget.h>
#include <vtkEventQtSlotConnect.h>

class FillSurfaceSelector : public QObject {
    Q_OBJECT
  public:
    explicit FillSurfaceSelector(QObject *parent = nullptr);
    virtual ~FillSurfaceSelector() override;
    void Execute();
    void SelectorOff();
    void SelectorOn();
    void SetVmtkRenderer(const QPointer<CustomVtkRenderer> value);
    void SetSurface(const vtkSmartPointer<vtkPolyData> value);
    vtkSmartPointer<vtkPolyData> GetSurface();
    qint32 GetFillCount();
  signals:
    void SignalFillFinish();
  public slots:

  private:
    void Initial();
    void FillerCallback();
    void STL2OFF(const QString off_filename, const qint32 num = 0);
    bool OFF2STL(const QString off_filename);
    vtkSmartPointer<vtkPolyData>CustomReader(std::istream &infile);
    bool CGALFunctionFill();
  private slots:
    void SlotKeyPressed(const QString &key);
    void SlotMouseClicked(const qint32 x, const qint32 y);
  private:
    bool enable_;
    bool first_connect_;
    bool own_renderer_;
    qint32 fill_id_;
    QPointer<CustomVtkRenderer> vmtk_renderer_;
    vtkSmartPointer<vtkPolyData> surface_;
    vtkSmartPointer<vtkPolyData> surface_region_;
    QList<vtkSmartPointer<vtkActor>> filled_actor_list_;
    QList<vtkSmartPointer<vtkPolyData>> filled_surface_list_;
    vtkSmartPointer<vtkActor> last_picked_actor_;
    vtkSmartPointer<vtkProperty> last_picked_property_;
};

#endif // CGALTHREADFILL_H
