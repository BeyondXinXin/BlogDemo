// 01 Frame includes
#include "vtkthreadform3dreconstruction.h"

// VTK includes
#include <vtkCommand.h>
#include <vtkPolyData.h>
#include <vtkPNGReader.h>
#include <vtkSTLWriter.h>
#include <vtkDecimatePro.h>
#include <vtkSmartPointer.h>
#include <vtkMarchingCubes.h>
#include <vtkSmoothPolyDataFilter.h>

VtkThreadForm3DReconstruction::VtkThreadForm3DReconstruction(QObject *parent) :
    VtkThread(parent) {
    this->Initial();
}

VtkThreadForm3DReconstruction::~VtkThreadForm3DReconstruction() {

}

void VtkThreadForm3DReconstruction::SetForm3DReconstructionValue(const QString &value) {
    this->reconstruction_value_ = value;
}

QString VtkThreadForm3DReconstruction::GetForm3DReconstructionValue() {
    return reconstruction_value_;
}

void VtkThreadForm3DReconstruction::run() {
    this->InitialResult();
    this->Execute();
}

void VtkThreadForm3DReconstruction::Initial() {
    reconstruction_value_ = "";
}

void VtkThreadForm3DReconstruction::Execute() {
    qDebug() << reconstruction_value_;
    vtkNew<vtkPNGReader>reader;
    vtkNew<vtkMarchingCubes> marchingcubes ;
    vtkNew<vtkDecimatePro> decimation;
    vtkNew<vtkSmoothPolyDataFilter> smoothfilter;
    vtkNew<vtkSTLWriter> vtk_writer_stl;
    QStringList reconstruction_value = this->reconstruction_value_.split("|");
    reader->SetFilePrefix( reconstruction_value.at(0).toLocal8Bit().data());

    QString tmppattern = QString("%s/%1.%2")
                         .arg(reconstruction_value.at(5))
                         .arg(reconstruction_value.at(4));

    reader->SetFilePattern(tmppattern.toLocal8Bit().data());
    reader ->SetDataExtent(0, 703, 0, 703, 1,
                           reconstruction_value.at(7).toInt() -
                           reconstruction_value.at(6).toInt());

    reader->SetDataSpacing(reconstruction_value.at(8).toInt(),
                           reconstruction_value.at(9).toInt(),
                           reconstruction_value.at(10).toInt());
    reader->Update();
    marchingcubes->SetInputConnection(reader->GetOutputPort());
    marchingcubes->SetValue(0, reconstruction_value.at(11).toInt());
    marchingcubes->SetValue(0, 1);
    marchingcubes->SetNumberOfContours(1);
    decimation->SetInputConnection(marchingcubes->GetOutputPort());
    double tmp_decimation = reconstruction_value.at(2).toDouble();
    decimation->SetTargetReduction(tmp_decimation);
    decimation->Update();
    smoothfilter->SetInputConnection(decimation->GetOutputPort());
    smoothfilter->SetNumberOfIterations(reconstruction_value.at(3).toInt());
    smoothfilter->Update();
    vtk_writer_stl->SetInputConnection(smoothfilter->GetOutputPort());
    QString tmp_name = QString("%1.stl")
                       .arg(reconstruction_value.at(1));
    vtk_writer_stl->SetFileName(tmp_name.toLocal8Bit().data());
    vtk_writer_stl->Write();
    reconstruction_value_ = tmp_name;
    this->SetResult(true);
}
