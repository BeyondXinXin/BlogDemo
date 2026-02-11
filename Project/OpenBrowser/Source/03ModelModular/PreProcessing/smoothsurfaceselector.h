#ifndef SMOOTHSURFACESELECTOR_H
#define SMOOTHSURFACESELECTOR_H

/*
 * polydata 区域平滑和裁剪
 */

// 01 Frame includes
#include "stable.h"

// 03 VTK includes
#include "customvtkrenderer.h"

// VTK includes
#include <vtkPolyData.h>
#include <vtkSeedWidget.h>
#include <vtkSmartPointer.h>
#include <vtkContourWidget.h>
#include <vtkEventQtSlotConnect.h>

class SmoothSurfaceSelector : public QObject {
    Q_OBJECT
  public:
    enum Method {
        SMOOTH,
        CUT,
    };
  public:
    explicit SmoothSurfaceSelector(QObject *parent = nullptr);
    virtual ~SmoothSurfaceSelector() override;
    void Execute();
    void SetMethod(const Method value);
    void SetVmtkRenderer(const QPointer<CustomVtkRenderer> value);
    void SetSurface(const vtkSmartPointer<vtkPolyData> value);
    vtkSmartPointer<vtkPolyData> GetSurface();
  signals:
    void SignalSelectorFinish();
  public slots:

  private:
    void Initial();
    void ClipCallback();
    void SmoothCallback();
    void UpdateSeedContour(const QList<QList<double>> &pos_list);
  private slots:
    void SlotKeyPressed(const QString &key);
    void SlotSeedChanged(vtkObject *caller, unsigned long vtk_event,
                         void *client_data, void *call_data);
  private:
    bool first_connect_;
    bool own_renderer_;
    Method method_;
    QPointer<CustomVtkRenderer> renderer_;
    vtkSmartPointer<vtkEventQtSlotConnect> vtk_connections_;
    vtkSmartPointer<vtkSeedWidget> seed_widget_;
    vtkSmartPointer<vtkContourWidget> contour_widget_;
    vtkSmartPointer<vtkPolyData> surface_;
    vtkSmartPointer<vtkPolyData> curved_surface_;
};

#endif // SMOOTHSURFACESELECTOR_H
