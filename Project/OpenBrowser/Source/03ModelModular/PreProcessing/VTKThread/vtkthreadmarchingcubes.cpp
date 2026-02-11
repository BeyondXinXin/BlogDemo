// 01 Frame includes
#include "vtkthreadmarchingcubes.h"

// VTK includes
#include <vtkImageCast.h>

VtkThreadMarchingCubes::VtkThreadMarchingCubes(QObject *parent) :
    VtkThread(parent) {
    this->Initial();
}

VtkThreadMarchingCubes::~VtkThreadMarchingCubes() {
}

void VtkThreadMarchingCubes::SetInputImageData(const vtkSmartPointer<vtkImageData> value) {
    this->imagedata_ = value;
}

void VtkThreadMarchingCubes::Setnum(int num) {
    marchcubes_num_ = num;
}

vtkSmartPointer<vtkMarchingCubes> VtkThreadMarchingCubes::Getmarchingcubes() {
    return  marchcubes_;
}

void VtkThreadMarchingCubes::run() {
    this->InitialResult();
    this->Execute();
}

void VtkThreadMarchingCubes::Initial() {
    this->setObjectName("VtkThreadMarchingCubes");
    this->imagedata_ = nullptr;
    marchcubes_num_ = 500;
    marchcubes_->SetNumberOfContours(1);
    marchcubes_->SetValue(0, marchcubes_num_);
    marchcubes_->Update();
}

void VtkThreadMarchingCubes::Execute() {
    marchcubes_->SetValue(0, marchcubes_num_);
    marchcubes_->SetInputData(imagedata_);
    marchcubes_->Update();
    this->SetResult(true);
}

