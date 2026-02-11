#ifndef CUSTOMSURFACEVIEWER_H
#define CUSTOMSURFACEVIEWER_H

/*
 * 自定义vtk view
 */

// 01 Frame includes
#include "stable.h"

// 03 VTK includes
#include "customvtkrenderer.h"

// VTK includes
#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkScalarBarActor.h>


class CustomSurfaceViewer : public QObject {
    Q_OBJECT
  public:

    enum Representation {
        SURFACE,
        WIREFRAME,
        EDGES
    };

  public:
    explicit CustomSurfaceViewer(QObject *parent = nullptr);
    virtual ~CustomSurfaceViewer() override;
    void Execute();
    void SetSurface(const vtkSmartPointer<vtkPolyData> value);
    void SetVmtkRenderer(const QPointer<CustomVtkRenderer> value);
    void SetColor(const QList<double> &value);
    vtkSmartPointer<vtkPolyData> GetSurface() const;
    vtkSmartPointer<vtkActor> GetActor() const;
  private:
    void Initial();
    void BuildView();
    void RepresentationCallback();
    void SetSurfaceRepresentation(const Representation value);
  private slots:
    void SlotKeyPressed(const QString &key);
  private:

    vtkSmartPointer<vtkPolyData> surface_;
    vtkSmartPointer<vtkActor> actor_;
    vtkSmartPointer<vtkActor2D> label_actor_;
    vtkSmartPointer<vtkScalarBarActor> scalar_bar_actor_;
    QPointer<CustomVtkRenderer> renderer_;

    bool first_connect_;
    bool own_renderer_;
    double opacity_;
    QList<double> color_;
    Representation representation_;


};

#endif // CUSTOMSURFACEVIEWER_H
