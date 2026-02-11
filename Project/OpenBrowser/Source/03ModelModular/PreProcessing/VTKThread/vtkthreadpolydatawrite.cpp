// 01 Frame includes
#include "vtkthreadpolydatawrite.h"

// VTK includes
#include <vtkCommand.h>
#include <vtkSTLWriter.h>
#include <vtkPLYWriter.h>
#include <vtkPolyDataWriter.h>
#include <vtkXMLPolyDataWriter.h>

VtkThreadPolyDataWrite::VtkThreadPolyDataWrite(QObject *parent) :
    VtkThread(parent) {
    this->Initial();
}

VtkThreadPolyDataWrite::~VtkThreadPolyDataWrite() {
}

void VtkThreadPolyDataWrite::Execute() {
    if (this->poly_data_ == nullptr) {
        qWarning() << "no Surface";
        return ;
    }
    if (this->output_file_name_ == "BROWSER") {
        this->output_file_name_ = QFileDialog::getOpenFileName(nullptr, "Input image");
        if (this->output_file_name_.isEmpty()) {
            qWarning() << "no OutputFileName.";
            return ;
        }
    }
    QFileInfo file_info(output_file_name_);
    QString extension = file_info.suffix();
    if (extension == "vtp") {
        this->format_ = Format::VTKXML;
    } else if (extension == "vtkxml") {
        this->format_ = Format::VTKXML;
    } else if (extension == "vtk") {
        this->format_ = Format::VTK;
    } else if (extension == "stl") {
        this->format_ = Format::STL;
    } else if (extension == "ply") {
        this->format_ = Format::PLY;
    } else if (extension == "tec") {
        this->format_ = Format::TECPLOT;
    } else if (extension == "dat") {
        this->format_ = Format::POINTDATA;
    } else {
        this->format_ = Format::UNKNOW;
    }
    if (this->format_ == VTK) {
        if (!this->WriteVtkSurfaceFile()) {
            return ;
        }
    } else if (this->format_ == STL) {
        if (!this->WriteStlSurfaceFile()) {
            return ;
        }
    }
    this->SetResult(true);
}

void VtkThreadPolyDataWrite::SetSurface(const vtkSmartPointer<vtkPolyData> value) {
    this->poly_data_ = value;
}

void VtkThreadPolyDataWrite::SetOutputFileName(const QString &value) {
    this->output_file_name_ = value;
}

void VtkThreadPolyDataWrite::run() {
    this->InitialResult();
    this->Execute();
}

void VtkThreadPolyDataWrite::Initial() {
    this->format_ = UNKNOW;
    this->mode_ = BINARY;
    this->output_file_name_ = "";
    this->setObjectName("VtkThreadPolyDataWrite");
}

bool VtkThreadPolyDataWrite::WriteVtkSurfaceFile() {
    if (this->output_file_name_.isEmpty()) {
        qWarning() << "no OutputFileName";
        return false;
    }
    qDebug() << "Writing VTK surface file";
    vtkNew<vtkPolyDataWriter> writer;
    writer->SetInputData(this->poly_data_);
    writer->SetFileName(this->output_file_name_.toLocal8Bit().data());
    writer->Update();
    if (this->mode_ == BINARY) {
        writer->SetFileTypeToBinary();
    } else if (this->mode_ == ASCII) {
        writer->SetFileTypeToASCII();
    }
    writer->AddObserver(vtkCommand::ProgressEvent,
                        this, &VtkThreadPolyDataWrite::PrintProgress);
    writer->Write();
    return true;
}

bool VtkThreadPolyDataWrite::WriteStlSurfaceFile() {
    if (this->output_file_name_.isEmpty()) {
        qWarning() << "no OutputFileName";
        return false;
    }
    qDebug() << "Writing VTK surface file";
    vtkNew<vtkSTLWriter> writer;
    writer->SetInputData(this->poly_data_);
    writer->SetFileName(this->output_file_name_.toLocal8Bit().data());
    writer->Update();
    if (this->mode_ == BINARY) {
        writer->SetFileTypeToBinary();
    } else if (this->mode_ == ASCII) {
        writer->SetFileTypeToASCII();
    }
    writer->AddObserver(vtkCommand::ProgressEvent,
                        this, &VtkThreadPolyDataWrite::PrintProgress);
    writer->Write();
    return true;
}
