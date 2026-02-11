// 01 Frame includes
#include "customvtkrenderer.h"

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include "vtkInteractorStyleTrackballCamera.h"

CustomVtkRenderer::CustomVtkRenderer(QVTKOpenGLWidget *widget, QObject *parent) :
    QObject(parent), widget_(widget) {
    this->Initial();
}

CustomVtkRenderer::~CustomVtkRenderer() {

}

void CustomVtkRenderer::Execute() {
    this->Initialize();
}

void CustomVtkRenderer::Deallocate() {
    this->render_window_interactor_ = nullptr;
    this->render_window_ = nullptr;
    this->renderer_ = nullptr;
}

void CustomVtkRenderer::ResetCamera() {
    this->renderer_->ResetCamera();
    this->render_window_->Render();
}

void CustomVtkRenderer::Render(bool interactive) {
    if (this->first_render_ && interactive) {
        this->render_window_interactor_->Initialize();
    }

    this->render_window_->Render();

    if (this->first_render_ && this->widget_ == nullptr && interactive) {
        this->render_window_interactor_->Start();
    }

    if (this->first_render_ && interactive) {
        this->first_render_ = false;
    }
}

void CustomVtkRenderer::SetBackground(const QList<double> &value) {
    if (value.size() == 3) {
        this->background_ = value;
    }
}


vtkSmartPointer<vtkRenderer> CustomVtkRenderer::GetRenderer() const {
    return  this->renderer_;
}

vtkSmartPointer<vtkGenericOpenGLRenderWindow> CustomVtkRenderer::GetRenderWindow() const {
    return  this->render_window_;
}

vtkSmartPointer<vtkRenderWindowInteractor>
CustomVtkRenderer::GetRenderWindowInteractor() const {
    return this->render_window_interactor_;
}

void CustomVtkRenderer::SlotKeyPressed(const QString &key) {
    if (key == "r" || key == "R") {
        this->ResetCamera();
    }
}

void CustomVtkRenderer::KeyPressCallback(vtkObject *caller,
        unsigned long vtk_event,
        void *client_data, void *call_data) {
    Q_UNUSED(caller)
    Q_UNUSED(vtk_event)
    Q_UNUSED(client_data)
    Q_UNUSED(call_data)
    QString key = this->render_window_interactor_->GetKeySym();
    emit SignalKeyPressed(key);
    return ;
}

void CustomVtkRenderer::MousePressCallBack(vtkObject *caller,
        unsigned long vtk_event,
        void *client_data, void *call_data) {
    Q_UNUSED(caller)
    Q_UNUSED(vtk_event)
    Q_UNUSED(client_data)
    Q_UNUSED(call_data)
    qint32 x, y;
    this->render_window_interactor_->GetEventPosition(x, y);
    emit SignalMouseClicked(x, y);
}


void CustomVtkRenderer::Initial() {
    this->first_render_ = true;
    this->renderer_ = nullptr;
    this->render_window_ = nullptr;
    this->background_ = {-1, -1, -1};
    this->setObjectName("customvtkrenderer");
}

void CustomVtkRenderer::Initialize() {
    if (this->renderer_ == nullptr) {
        this->renderer_ = vtkSmartPointer<vtkRenderer>::New();
        this->renderer_->SetBackground(this->background_[0],
                                       this->background_[1],
                                       this->background_[2]);
    }
    this->render_window_ = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    if (this->widget_ != nullptr) {
        this->widget_->SetRenderWindow(this->render_window_);
    }
    this->render_window_->AddRenderer(this->renderer_);
    this->render_window_->SetPointSmoothing(true);
    this->render_window_->SetLineSmoothing(true);
    this->render_window_->SetPolygonSmoothing(false);
    this->renderer_->Render();

    this->render_window_interactor_ =
        this->widget_->GetRenderWindow()->GetInteractor();

    this->render_window_interactor_->SetInteractorStyle(
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New());
    this->render_window_interactor_->GetInteractorStyle()->KeyPressActivationOff();

    this->connections_ = vtkSmartPointer<vtkEventQtSlotConnect>::New();
    connections_->Connect(this->render_window_interactor_,
                          vtkCommand::KeyPressEvent,
                          this, SLOT(KeyPressCallback(vtkObject *,
                                     unsigned long, void *, void *)));
    connections_->Connect(this->render_window_interactor_,
                          vtkCommand::LeftButtonPressEvent,
                          this, SLOT(MousePressCallBack(vtkObject *,
                                     unsigned long, void *, void *)));
    connect(this, &CustomVtkRenderer::SignalKeyPressed,
            this, &CustomVtkRenderer::SlotKeyPressed);
}






















