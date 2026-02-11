#include "readivus.h"
#include "ui_readivus.h"
#include "vtkWindowToImageFilter.h"

ReadIVUS::ReadIVUS(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReadIVUS) {
    ui->setupUi(this);
    this->setFixedSize(880, 570);
    angle = 0;
    ui->slider->move(300, 0);
    connect(ui->slider, &QSlider::valueChanged,
            this, &ReadIVUS::SlotTransverseCurrentIndex);

    connect(ui->spinbox_l, SIGNAL(valueChanged(int)), ui->slider, SLOT(setValue(int)));
    connect(ui->slider, SIGNAL(valueChanged(int)), ui->spinbox_l, SLOT(setValue(int)));


    // 左侧
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow_transverse;
    imageViewer->SetRenderWindow(renderWindow_transverse);
    ui->transverse->SetRenderWindow(imageViewer->GetRenderWindow());
    imageViewer->SetupInteractor(
        ui->transverse->GetRenderWindow()->GetInteractor());
    imageViewer->GetRenderer()->SetBackground(90 / 255.0, 90 / 255.0, 90 / 255.0);
    vtkNew<vtkInteractorStyleImage> transverse_style;
    ui->transverse->GetRenderWindow()->GetInteractor()->
    SetInteractorStyle(transverse_style);
    vtk_connections_->Connect(transverse_style,
                              vtkCommand::MouseWheelForwardEvent,
                              this, SLOT(SlotLeftForward()));
    vtk_connections_->Connect(transverse_style,
                              vtkCommand::MouseWheelBackwardEvent,
                              this, SLOT(SlotLeftBackword()));

    // 右侧
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow_median_sagittal;
    ui->median_sagittal->SetRenderWindow(renderWindow_median_sagittal);
    ui->median_sagittal->GetRenderWindow()->AddRenderer(select_render_);
    select_render_->SetBackground(90 / 255.0, 90 / 255.0, 90 / 255.0);
    vtkNew<vtkInteractorStyleImage> median_sagittal_style;
    ui->median_sagittal->GetRenderWindow()->GetInteractor()->
    SetInteractorStyle(median_sagittal_style);
    vtk_connections_->Connect(median_sagittal_style,
                              vtkCommand::LeftButtonReleaseEvent,
                              this, SLOT(SlotEmpty()));
    vtk_connections_->Connect(median_sagittal_style,
                              vtkCommand::LeftButtonPressEvent,
                              this, SLOT(SlotEmpty()));
    vtk_connections_->Connect(median_sagittal_style,
                              vtkCommand::MouseWheelForwardEvent,
                              this, SLOT(SlotRightForward()));
    vtk_connections_->Connect(median_sagittal_style,
                              vtkCommand::MouseWheelBackwardEvent,
                              this, SLOT(SlotRightBackword()));

}

ReadIVUS::~ReadIVUS() {
    delete ui;
}



void ReadIVUS::on_pushButton_clicked() {

    QString tmp_file;
    tmp_file =
        QFileDialog::getOpenFileName(
            nullptr, "选择文件", QCoreApplication::applicationDirPath(), "*.dcm *.*");
    QFileInfo file_info(tmp_file);
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
        return ;
    }
    typedef itk::ImageToVTKImageFilter< ImageType> itkTovtkFilterType;
    itkTovtkFilterType::Pointer itkTovtkImageFilter = itkTovtkFilterType::New();
    itkTovtkImageFilter->SetInput(reader->GetOutput());
    itkTovtkImageFilter->Update();

    int imageDims[3];
    imagedata_->DeepCopy(itkTovtkImageFilter->GetOutput());
    imagedata_->SetSpacing(1.5, 1.5, 0.5);
    imagedata_->GetDimensions(imageDims);//获取长宽高
    ui->slider->setMaximum(imageDims[2]);
    ui->spinbox_l->setMaximum(imageDims[2]);



    int extent[6];
    double spacing[3];
    double origin[3];
    imagedata_->GetExtent(extent);
    imagedata_->GetSpacing(spacing);
    imagedata_->GetOrigin(origin);
    double center[3];
    center[0] = origin[0] + spacing[0] * 0.5 * (extent[0] + extent[1]);
    center[1] = origin[1] + spacing[1] * 0.5 * (extent[2] + extent[3]);
    center[2] = origin[2] + spacing[2] * 0.5 * (extent[4] + extent[5]);
    double axialElements[16] = {
        cos(angle * 3.14 / 180),   0,   sin(angle * 3.14 / 180),   0,
        sin(angle * 3.14 / 180),  0,   cos(angle * 3.14 / 180),   0,
        0,     -1,  0,     0,
        0,     0,   0,     1
    };
    vtkNew< vtkMatrix4x4 > resliceAxes ;
    resliceAxes->DeepCopy(axialElements);
    resliceAxes->SetElement(0, 3, center[0]);
    resliceAxes->SetElement(1, 3, center[1]);
    resliceAxes->SetElement(2, 3, center[2]);

    reslice->SetInputData(imagedata_);
    reslice->SetOutputDimensionality(2);
    reslice->SetResliceAxes(resliceAxes);
    reslice->SetInterpolationModeToLinear();
    reslice->Update();

    imgActor->SetInputData(reslice->GetOutput());
    select_render_->AddActor(imgActor);

    // 左侧
    imageViewer->SetInputData(imagedata_);
    imageViewer->SetSlice(5000);
    ui->slider->setValue(imageViewer->GetSlice());
    vtkSmartPointer<vtkCamera> camera_reight = this->select_render_->GetActiveCamera();
    camera_reight->ParallelProjectionOn();
    double yd_right = (extent[5] - extent[4] + 1) * spacing[1];
    double d = camera_reight->GetDistance();
    camera_reight->SetFocalPoint(center[0], center[1], 0.0);
    camera_reight->SetPosition(center[0], center[1], d);
    camera_reight->SetParallelScale(0.5 * (yd_right - 1));
    ui->transverse->GetRenderWindow()->Render();

    // 右侧
    vtkSmartPointer<vtkCamera> camera_left = this->select_render_->GetActiveCamera();
    double origin_left[3];
    double spacing_left[3];
    int extent_left[6];
    reslice->GetOutput()->GetOrigin(origin_left);
    reslice->GetOutput()->GetSpacing(spacing_left);
    reslice->GetOutput()->GetExtent(extent_left);
    camera_left->ParallelProjectionOn();
    double xc_left =
        origin_left[0] + 0.5 * (extent_left[0] + extent_left[1]) * spacing_left[0];
    double yc_left =
        origin_left[1] + 0.5 * (extent_left[2] + extent_left[3]) * spacing_left[1];
    double yd_left = (extent_left[3] - extent_left[2] + 1) * spacing_left[1];
    double d_left = camera_left->GetDistance();
    camera_left->SetFocalPoint(xc_left, yc_left, 0.0);
    camera_left->SetPosition(xc_left, yc_left, d_left);
    camera_left->SetParallelScale(0.5 * (yd_left - 1));
    ui->median_sagittal->GetRenderWindow()->Render();
}



void ReadIVUS::SlotLeftForward() {
    imageViewer->SetSlice(imageViewer->GetSlice() + 1);
    ui->slider->setValue(imageViewer->GetSlice());
}

void ReadIVUS::SlotLeftBackword() {
    imageViewer->SetSlice(imageViewer->GetSlice() - 1);
    ui->slider->setValue(imageViewer->GetSlice());
}

void ReadIVUS::SlotRightForward() {
    if (angle < 180) {
        angle += 1;
    }
    ui->spinbox_r->setValue(angle);
    LeftChangeAngle();
}

void ReadIVUS::SlotRightBackword() {
    if (angle > 1) {
        angle -= 1;
    }
    ui->spinbox_r->setValue(angle);
    LeftChangeAngle();
}

void ReadIVUS::SlotTransverseCurrentIndex(const int value) {
    imageViewer->SetSlice(value);
}

void ReadIVUS::LeftChangeAngle() {
    int extent[6];
    double spacing[3];
    double origin[3];
    imagedata_->GetExtent(extent);
    imagedata_->GetSpacing(spacing);
    imagedata_->GetOrigin(origin);
    double center[3];
    center[0] = origin[0] + spacing[0] * 0.5 * (extent[0] + extent[1]);
    center[1] = origin[1] + spacing[1] * 0.5 * (extent[2] + extent[3]);
    center[2] = origin[2] + spacing[2] * 0.5 * (extent[4] + extent[5]);
    double axialElements[16] = {
        cos(angle * 3.14 / 180),   0,   sin(angle * 3.14 / 180),   0,
        sin(angle * 3.14 / 180),  0,   cos(angle * 3.14 / 180),   0,
        0,     -1,  0,     0,
        0,     0,   0,     1
    };
    vtkNew< vtkMatrix4x4 > resliceAxes ;
    resliceAxes->DeepCopy(axialElements);
    resliceAxes->SetElement(0, 3, center[0]);
    resliceAxes->SetElement(1, 3, center[1]);
    resliceAxes->SetElement(2, 3, center[2]);
    reslice->SetInputData(imagedata_);
    reslice->SetOutputDimensionality(2);
    reslice->SetResliceAxes(resliceAxes);
    reslice->SetInterpolationModeToLinear();
    reslice->Update();
    imgActor->SetInputData(reslice->GetOutput());
    ui->median_sagittal->GetRenderWindow()->Render();

}

void ReadIVUS::on_spinbox_r_editingFinished() {
    angle = ui->spinbox_r->value();
    LeftChangeAngle();
}

void ReadIVUS::on_pushButton_2_clicked() {
    vtkNew<vtkWindowToImageFilter> filter_median_sagittal;
    filter_median_sagittal->SetInputBufferTypeToRGBA();
    filter_median_sagittal->ReadFrontBufferOff();
    filter_median_sagittal->SetInput(ui->median_sagittal->GetRenderWindow());
    filter_median_sagittal->Update();
    QImage image_median_sagittal
        = VtkImageDataToQImage(filter_median_sagittal->GetOutput());
    QString median_sagittal = QString("median_sagittal%1.png")
                              .arg(ui->spinbox_l->value());
    image_median_sagittal.save(median_sagittal);


    vtkNew<vtkWindowToImageFilter> filter_transverse;
    filter_median_sagittal->SetInputBufferTypeToRGBA();
    filter_median_sagittal->ReadFrontBufferOff();
    filter_median_sagittal->SetInput(ui->transverse->GetRenderWindow());
    filter_median_sagittal->Update();

    QImage image_transverse = VtkImageDataToQImage(filter_median_sagittal->GetOutput());
    QString transverse = QString("transverse%1.png")
                         .arg(ui->spinbox_r->value());
    image_median_sagittal.save(median_sagittal);
    image_transverse.save(transverse);
}

QImage ReadIVUS::VtkImageDataToQImage(vtkSmartPointer<vtkImageData> imageData) {
    if (!imageData) {
        qWarning() << "image data is null";
        return QImage();
    }
    /// \todo retrieve just the UpdateExtent
    qint32 width = imageData->GetDimensions()[0];
    qint32 height = imageData->GetDimensions()[1];
    QImage image(width, height, QImage::Format_RGB32);
    QRgb *rgbPtr = reinterpret_cast<QRgb *>(image.bits()) + width * (height - 1);
    unsigned char *colorsPtr =
        reinterpret_cast<unsigned char *>(imageData->GetScalarPointer());

    // Loop over the vtkImageData contents.
    for (qint32 row = 0; row < height; row++) {
        for (qint32 col = 0; col < width; col++) {
            // Swap the vtkImageData RGB values with an equivalent QColor
            *(rgbPtr++) = QColor(colorsPtr[0], colorsPtr[1], colorsPtr[2]).rgb();
            colorsPtr += imageData->GetNumberOfScalarComponents();
        }
        rgbPtr -= width * 2;
    }
    return image;
}
