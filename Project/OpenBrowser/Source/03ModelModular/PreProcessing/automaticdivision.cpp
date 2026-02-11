// 01 Frame includes
#include "automaticdivision.h"

// VTK includes
#include <vtkActor.h>
#include  <vtkCamera.h>
#include  <vtkPolyData.h>
#include  <vtkProperty.h>
#include  <vtkRenderer.h>
#include  <vtkDecimatePro.h>
#include  <vtkNamedColors.h>
#include  <vtkRenderWindow.h>
#include  <vtkSphereSource.h>
#include  <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include  <vtkPolyDataMapper.h>
#include  <vtkTriangleFilter.h>
#include  <vtkXMLPolyDataReader.h>
#include  <vtkRenderWindowInteractor.h>
#include <vtkPolyDataConnectivityFilter.h>


AutomaticDivision::AutomaticDivision(QObject *parent) : QObject(parent) {
    this->Initial();
}

AutomaticDivision::~AutomaticDivision() {
}

void AutomaticDivision::Execute() {
    if (this->surface_ != nullptr) {
        vtkNew<vtkPolyDataConnectivityFilter> connectivity_filter;
        connectivity_filter->SetInputData(this->surface_);
        connectivity_filter->SetExtractionModeToLargestRegion();
        connectivity_filter->AddSeed(100);
        connectivity_filter->Update();
        this->surface_ = connectivity_filter->GetOutput();
        emit SignalClippedFinish();
    }
}

void AutomaticDivision::SetSurface(const vtkSmartPointer<vtkPolyData> value) {
    this->surface_ = value;
}

vtkSmartPointer<vtkPolyData> AutomaticDivision::GetSurface() {
    return this->surface_;
}

void AutomaticDivision::Initial() {
    this->surface_ = nullptr;
}
