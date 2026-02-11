// 01 Frame includes
#include "vtkthreadimagedataread.h"

// VTK includes
#include "vtkImageAlgorithm.h"

// ITK includes
#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkImageSeriesReader.h"
#include "itkGDCMSeriesFileNames.h"
#include <itkImageToVTKImageFilter.h>
#include "itkImageToVTKImageFilter.h"




VtkThreadImageDataRead::VtkThreadImageDataRead(QObject *parent) :
    VtkThread(parent) {
    this->Initial();
}


VtkThreadImageDataRead::~VtkThreadImageDataRead() {
}

void VtkThreadImageDataRead::SetInputFileName(const QString &value) {
    this->input_filename_ = value;
}

vtkSmartPointer<vtkImageData> VtkThreadImageDataRead::GetImageData()  {
    return imagedata_;
}

void VtkThreadImageDataRead::run() {
    this->InitialResult();
    this->Execute();
}

void VtkThreadImageDataRead::Initial() {
    this->format_ = Format::UNKNOW;
    this->data_spacing_ = {1.0, 1.0, 1.0};
    this->data_origin_ = {0.0, 0.0, 0.0};
    this->setObjectName("vtkthreadimagedataread");
}

void VtkThreadImageDataRead::Execute() {
    //判断文件类型
    QFileInfo file_info(input_filename_);
    QString extension = file_info.suffix();
    if (extension == "vti") {
        this->format_ = Format::VTKXML;
    } else if (extension == "vtkxml") {
        this->format_ = Format::VTKXML;
    } else if (extension == "vtk") {
        this->format_ = Format::VTK;
    } else if (extension == "dcm") {
        this->format_ = Format::DICOM;
    } else if (extension == "raw") {
        this->format_ = Format::RAW;
    } else if (extension == "mhd") {
        this->format_ = Format::META;
    } else if (extension == "mha") {
        this->format_ = Format::META;
    } else if (extension == "tif") {
        this->format_ = Format::TIFF;
    } else if (extension == "png") {
        this->format_ = Format::PNG;
    } else {
        this->format_ = Format::UNKNOW;
    }
    //读取文件
    if (this->format_ == Format::DICOM) {
        if (!this->ReadDicom()) {
            return ;
        }
    }
    this->SetResult(true);
}


bool VtkThreadImageDataRead::ReadDicom() {
    if (this->input_filename_.isEmpty()) {
        qWarning() << "no InputFileName";
        return false;
    }
    QFileInfo file_info(this->input_filename_);
    QString extension = file_info.path();
    using PixelType = float;
    constexpr unsigned int Dimension = 3;
    using ImageType = itk::Image< PixelType, Dimension >;
    using ReaderType = itk::ImageSeriesReader< ImageType >;
    using ImageIOType = itk::GDCMImageIO;
    using NamesGeneratorType = itk::GDCMSeriesFileNames;
    ReaderType::Pointer reader = ReaderType::New();
    ImageIOType::Pointer dicomIO = ImageIOType::New();
    reader->SetImageIO(dicomIO);
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
    nameGenerator->SetUseSeriesDetails(true);
    nameGenerator->SetDirectory(extension.toLocal8Bit().data());
    using SeriesIdContainer = std::vector< std::string >;
    const SeriesIdContainer &seriesUID = nameGenerator->GetSeriesUIDs();
    auto seriesItr = seriesUID.begin();
    auto seriesEnd = seriesUID.end();
    using FileNamesContainer = std::vector< std::string >;
    FileNamesContainer fileNames ;
    std::string seriesIdentifier;
    while (seriesItr != seriesEnd) {
        seriesIdentifier = seriesItr->c_str();
        fileNames = nameGenerator->GetFileNames(seriesIdentifier);
        ++seriesItr;
    }
    reader->SetFileNames(fileNames);
    try {
        reader->Update();
    } catch (itk::ExceptionObject &ex) {
        std::cout << ex << std::endl;
        return EXIT_FAILURE;
    }
    typedef itk::ImageToVTKImageFilter< ImageType> itkTovtkFilterType;
    itkTovtkFilterType::Pointer itkTovtkImageFilter = itkTovtkFilterType::New();
    itkTovtkImageFilter->SetInput(reader->GetOutput());
    itkTovtkImageFilter->Update();
    this->imagedata_->DeepCopy(itkTovtkImageFilter->GetOutput());
    return true;
}


















