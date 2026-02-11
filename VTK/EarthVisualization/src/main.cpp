#include <QApplication>
#include <QHBoxLayout>
#include <QScreen>

#include <vtkOpenGLRenderWindow.h>
#include <vtkTexturedSphereSource.h>
#include <vtkTexture.h>
#include <vtkTransformTextureCoords.h>
#include <vtkPolyDataMapper.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkSampleFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolumeProperty.h>

#include "QVtkWidget.h"
#include "vtkEarthSphere.h"

vtkSmartPointer<vtkActor> CreateTexturedEarth(const std::string & imagePath);
vtkSmartPointer<vtkVolume> CreateVolumeEarth(const std::string & topoPath, const std::string & bathPath);

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);

    vtkOpenGLRenderWindow::SetGlobalMaximumNumberOfMultiSamples(0);

    QWidget wid;
    QHBoxLayout layMain;
    QVtkWidget * widget1 = new QVtkWidget();
    QVtkWidget * widget2 = new QVtkWidget();

    layMain.addWidget(widget1, 2);
    layMain.addWidget(widget2, 2);

    int widHeight = 600;
    wid.setLayout(&layMain);
    wid.resize(widHeight * 2, widHeight);
    wid.setWindowTitle(" ");
    wid.show();

    widget1->SetBorderColor(QColor(180, 80, 80));
    widget1->Renderer()->SetBackground(.15, .15, .15);
    widget2->SetBorderColor(QColor(80, 180, 80));
    widget2->Renderer()->SetBackground(.15, .15, .15);

    vtkSmartPointer<vtkActor> actor1 = CreateTexturedEarth("D://earth.jpg");
    widget1->Renderer()->AddActor(actor1);

    vtkSmartPointer<vtkVolume> volume1 = CreateVolumeEarth("D://topography.jpg", "D://bathymetry.jpg");
    widget2->Renderer()->AddViewProp(volume1);

    return QApplication::exec();
}

vtkSmartPointer<vtkActor> CreateTexturedEarth(const std::string & imagePath)
{
    vtkNew<vtkTexturedSphereSource> sphere;
    sphere->SetPhiResolution(32);
    sphere->SetRadius(10 * 6.371e6 / 6371 * 10e3); // 地球半径
    sphere->SetThetaResolution(32);

    vtkNew<vtkImageReader2Factory> readerFactory;
    vtkSmartPointer<vtkImageReader2> imageReader;
    imageReader.TakeReference(readerFactory->CreateImageReader2(imagePath.c_str()));
    imageReader->SetFileName(imagePath.c_str());

    vtkNew<vtkTexture> texture;
    texture->SetInputConnection(imageReader->GetOutputPort());

    vtkNew<vtkTransformTextureCoords> transformTexture;
    transformTexture->SetInputConnection(sphere->GetOutputPort());
    transformTexture->SetPosition(0, 0, 0);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(transformTexture->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->SetTexture(texture);

    return actor;
}

vtkSmartPointer<vtkVolume> CreateVolumeEarth(const std::string & topoPath, const std::string & bathPath)
{
    vtkNew<vtkEarthSphere> earthSphere;
    earthSphere->SetTopographyTexture(topoPath);
    earthSphere->SetBathymetryTexture(bathPath);

    vtkNew<vtkSampleFunction> sampleFunction;
    sampleFunction->SetImplicitFunction(earthSphere);
    sampleFunction->SetSampleDimensions(255, 255, 255);
    sampleFunction->SetModelBounds(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    sampleFunction->Update();

    vtkNew<vtkSmartVolumeMapper> volumeMapper;
    volumeMapper->SetInputConnection(sampleFunction->GetOutputPort());

    vtkNew<vtkPiecewiseFunction> opacityFunction;
    opacityFunction->AddPoint(-8100, 0.0);
    opacityFunction->AddPoint(-8000, 1.0);
    opacityFunction->AddPoint(6400, 1.0);
    opacityFunction->AddPoint(6500, .0);

    vtkNew<vtkColorTransferFunction> colorFunction;
    // 海洋深度 (-8000 到 0)
    colorFunction->AddRGBPoint(-8000, 0.0, 0.0, 0.2); // 深海沟
    colorFunction->AddRGBPoint(-4000, 0.0, 0.1, 0.4); // 深海
    colorFunction->AddRGBPoint(-2000, 0.0, 0.2, 0.6); // 中深海
    colorFunction->AddRGBPoint(-500, 0.1, 0.4, 0.8); // 浅海
    colorFunction->AddRGBPoint(-100, 0.15, 0.5, 0.85); // 近海
    colorFunction->AddRGBPoint(0, 0.3, 0.6, 0.3); // 海岸线(改为绿色调)
    // 陆地高程 (0 到 6400)
    colorFunction->AddRGBPoint(50, 0.3, 0.65, 0.25); // 海岸湿地
    colorFunction->AddRGBPoint(200, 0.2, 0.75, 0.2); // 低地平原(鲜绿)
    colorFunction->AddRGBPoint(500, 0.4, 0.85, 0.3); // 平原(亮绿,突出)
    colorFunction->AddRGBPoint(800, 0.5, 0.8, 0.35); // 高平原
    colorFunction->AddRGBPoint(1200, 0.65, 0.75, 0.35); // 丘陵过渡
    colorFunction->AddRGBPoint(1800, 0.75, 0.7, 0.3); // 丘陵(黄绿)
    colorFunction->AddRGBPoint(2500, 0.7, 0.55, 0.25); // 低山(褐色)
    colorFunction->AddRGBPoint(3500, 0.65, 0.45, 0.2); // 中山
    colorFunction->AddRGBPoint(4500, 0.6, 0.35, 0.15); // 高山(深褐)
    colorFunction->AddRGBPoint(5000, 0.7, 0.7, 0.7); // 雪线开始
    colorFunction->AddRGBPoint(5500, 0.85, 0.85, 0.85); // 积雪
    colorFunction->AddRGBPoint(6400, 1.0, 1.0, 1.0); // 雪峰(纯白)

    vtkNew<vtkVolumeProperty> volumeProperty;
    volumeProperty->SetScalarOpacity(opacityFunction);
    volumeProperty->SetColor(colorFunction);

    vtkNew<vtkVolume> volume;
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);

    return volume;
}
