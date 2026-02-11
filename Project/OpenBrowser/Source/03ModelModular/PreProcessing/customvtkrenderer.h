#ifndef CUSTOMVTKRENDERER_H
#define CUSTOMVTKRENDERER_H

/*
 * 自定义vtk randerer
 */

// 01 Frame includes
#include "stable.h"

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <QVTKOpenGLWidget.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkRenderWindowInteractor.h>

class CustomVtkRenderer : public QObject {
    Q_OBJECT
  public:
    explicit CustomVtkRenderer(QVTKOpenGLWidget *widget = nullptr,
                               QObject *parent = nullptr);
    virtual ~CustomVtkRenderer() override;
    void Execute();
    void Deallocate();
    void ResetCamera();
    void Render(bool interactive = true);
    void SetBackground(const QList<double> &value);
    vtkSmartPointer<vtkRenderer> GetRenderer() const;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> GetRenderWindow() const;
    vtkSmartPointer<vtkRenderWindowInteractor> GetRenderWindowInteractor() const;
  signals:
    void SignalKeyPressed(const QString &key);
    void SignalMouseClicked(const qint32 x, const qint32 y);
  private slots:
    void SlotKeyPressed(const QString &key);
    void KeyPressCallback(vtkObject *caller,
                          unsigned long vtk_event,
                          void *client_data, void *call_data);
    void MousePressCallBack(vtkObject *caller,
                            unsigned long vtk_event,
                            void *client_data, void *call_data);


  private:
    void Initial();
    void Initialize();
  private:
    bool first_render_;
    QPointer<QVTKOpenGLWidget> widget_;
    vtkSmartPointer<vtkRenderer> renderer_;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> render_window_;
    vtkSmartPointer<vtkRenderWindowInteractor> render_window_interactor_;
    vtkSmartPointer<vtkEventQtSlotConnect> connections_;
    QList<double> background_;

};




#endif // CUSTOMVTKRENDERER_H
