// 01 Frame includes
#include "customsurfaceviewer.h"

// VTK includes
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkCellCenters.h>
#include <vtkLookupTable.h>
#include <vtkTextProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkScalarsToColors.h>
#include <vtkLabeledDataMapper.h>
#include <vtkColorTransferFunction.h>
#include <vtkGenericOpenGLRenderWindow.h>

CustomSurfaceViewer::CustomSurfaceViewer(QObject *parent) : QObject(parent) {
    this->Initial();
}

CustomSurfaceViewer::~CustomSurfaceViewer() {

}

void CustomSurfaceViewer::Execute() {
    if (this->surface_ == nullptr) {
        qWarning() << "no Surface";
        return ;
    }
    this->BuildView();
}

void CustomSurfaceViewer::SetSurface(const vtkSmartPointer<vtkPolyData> value) {
    this->surface_ = value;
}

void CustomSurfaceViewer::SetVmtkRenderer(const QPointer<CustomVtkRenderer> value) {
    this->renderer_ = value;
}

void CustomSurfaceViewer::SetColor(const QList<double> &value) {
    if (value.size() == 3) {
        this->color_ = value;
    }
}

vtkSmartPointer<vtkPolyData> CustomSurfaceViewer::GetSurface() const {
    return this->surface_;
}

vtkSmartPointer<vtkActor> CustomSurfaceViewer::GetActor() const {
    return this->actor_;
}

void CustomSurfaceViewer::Initial() {
    this->first_connect_ = true;
    this->surface_ = nullptr;
    this->renderer_ = nullptr;
    this->own_renderer_ = false;
    this->opacity_ = 1.0;
    this->color_ = {-1.0, -1.0, -1.0};
    this->representation_ = SURFACE;

    this->actor_ = nullptr;
    this->label_actor_ = nullptr;
    this->scalar_bar_actor_ = nullptr;

    this->setObjectName("CustomSurfaceViewer");
}

void CustomSurfaceViewer::BuildView() {
    if (this->renderer_ == nullptr) {
        this->renderer_ = new CustomVtkRenderer();
        this->renderer_->Execute();
        this->own_renderer_ = true;
    }

    if (this->actor_ != nullptr) {
        this->renderer_->GetRenderer()->RemoveActor(this->actor_);
    }

    if (this->scalar_bar_actor_ != nullptr) {
        this->renderer_->GetRenderer()->RemoveActor(this->scalar_bar_actor_);
    }

    if (this->surface_ != nullptr) {
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(this->surface_);
        mapper->ScalarVisibilityOff();


        this->actor_ = vtkSmartPointer<vtkActor>::New();
        this->actor_->SetMapper(mapper);
        if (this->color_[0] >= 0) {
            this->actor_->GetProperty()->SetColor(this->color_[0],
                                                  this->color_[1],
                                                  this->color_[2]);
        }
        this->actor_->GetProperty()->SetOpacity(this->opacity_);
        this->actor_->GetProperty()->SetLineWidth(1);

        this->actor_->GetProperty()->SetInterpolationToFlat();

        this->SetSurfaceRepresentation(this->representation_);
        this->renderer_->GetRenderer()->AddActor(this->actor_);
        if (first_connect_) {
            connect(this->renderer_, &CustomVtkRenderer::SignalKeyPressed,
                    this, &CustomSurfaceViewer::SlotKeyPressed);
            first_connect_ = false;
        }
    }
    this->renderer_->Render();
    if (this->own_renderer_) {
        this->renderer_->Deallocate();
    }

}

void CustomSurfaceViewer::RepresentationCallback() {
    Representation representation = SURFACE;
    if (this->representation_ == SURFACE) {
        representation = EDGES;
    } else if (this->representation_ == EDGES) {
        representation = WIREFRAME;
    } else if (this->representation_ == WIREFRAME) {
        representation = SURFACE;
    }
    this->SetSurfaceRepresentation(representation);
    this->renderer_->GetRenderWindow()->Render();
}

void CustomSurfaceViewer::SetSurfaceRepresentation(
    const CustomSurfaceViewer::Representation value) {
    if (value == SURFACE) {
        this->actor_->GetProperty()->SetRepresentationToSurface();
        this->actor_->GetProperty()->EdgeVisibilityOff();
    } else if (value == EDGES) {
        this->actor_->GetProperty()->SetRepresentationToSurface();
        this->actor_->GetProperty()->EdgeVisibilityOn();
    } else if (value == WIREFRAME) {
        this->actor_->GetProperty()->SetRepresentationToWireframe();
        this->actor_->GetProperty()->EdgeVisibilityOff();
    }
    this->representation_ = value;
}

void CustomSurfaceViewer::SlotKeyPressed(const QString &key) {
    if (key == "e" || key == "E") {
        this->RepresentationCallback();
    }
}
