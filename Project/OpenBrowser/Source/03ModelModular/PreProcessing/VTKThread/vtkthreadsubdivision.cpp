// 01 Frame includes
#include "vtkthreadsubdivision.h"

// VTK includes
#include <vtkCommand.h>
#include <vtkLoopSubdivisionFilter.h>
#include <vtkLinearSubdivisionFilter.h>
#include <vtkButterflySubdivisionFilter.h>




VtkThreadSubdivision::VtkThreadSubdivision(QObject *parent) : VtkThread(parent) {
    this->Initial();
}

VtkThreadSubdivision::~VtkThreadSubdivision() {
}

void VtkThreadSubdivision::Execute() {
    qDebug();
    if (this->surface_ == nullptr) {
        qWarning() << "No input surface";
        return ;
    }
    emit SignalVtkThreadProgressOut("正在细分模型...", -1);
    if (this->method_ == LINEAR) {
        vtkNew<vtkLinearSubdivisionFilter> subdivision_filter;
        subdivision_filter->SetInputData(this->surface_);
        subdivision_filter->SetNumberOfSubdivisions(this->number_of_subdivisions_);
        subdivision_filter->AddObserver(vtkCommand::ProgressEvent,
                                        this, &VtkThreadSubdivision::PrintProgress);
        subdivision_filter->Update();
        this->surface_ = subdivision_filter->GetOutput();
    } else if (this->method_ == BUTTERFLY) {
        vtkNew<vtkButterflySubdivisionFilter> subdivision_filter;
        subdivision_filter->SetInputData(this->surface_);
        subdivision_filter->SetNumberOfSubdivisions(this->number_of_subdivisions_);
        subdivision_filter->AddObserver(vtkCommand::ProgressEvent,
                                        this, &VtkThreadSubdivision::PrintProgress);
        subdivision_filter->Update();
        this->surface_ = subdivision_filter->GetOutput();
    } else if (this->method_ == LOOP) {
        vtkNew<vtkLoopSubdivisionFilter> subdivision_filter;
        subdivision_filter->SetInputData(this->surface_);
        subdivision_filter->SetNumberOfSubdivisions(this->number_of_subdivisions_);
        subdivision_filter->AddObserver(vtkCommand::ProgressEvent,
                                        this, &VtkThreadSubdivision::PrintProgress);
        subdivision_filter->Update();
        this->surface_ = subdivision_filter->GetOutput();
    } else {
        qWarning() << "Unsupported subdivision method";
        return ;
    }
    this->SetResult(true);
}

void VtkThreadSubdivision::SetSurface(const vtkSmartPointer<vtkPolyData> value) {
    this->surface_ = value;
}

void VtkThreadSubdivision::SetNumberOfSubdivisions(const qint32 value) {
    if (value >= 0) {
        this->number_of_subdivisions_ = value;
    }
}

void VtkThreadSubdivision::SetMethod(const VtkThreadSubdivision::Method value) {
    this->method_ = value;
}


vtkSmartPointer<vtkPolyData> VtkThreadSubdivision::GetSurface() const {
    return this->surface_;
}

void VtkThreadSubdivision::run() {
    this->InitialResult();
    this->Execute();
}

void VtkThreadSubdivision::Initial() {
    this->surface_ = nullptr;
    this->number_of_subdivisions_ = 1;
    this->method_ = LINEAR;
    this->setObjectName("VtkThreadSubdivision");
}
