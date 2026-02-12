#include <QApplication>
#include <QHBoxLayout>

#include <vtkNrrdReader.h>
#include <vtkMatrix4x4.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkCamera.h>
#include <vtkDataSet.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkAbstractVolumeMapper.h>
#include <vtkImageCast.h>
#include <vtkBoxWidget2.h>
#include <vtkBoxRepresentation.h>
#include <vtkPlanes.h>
#include <vtkMatrix3x3.h>

#include "QVtkWidget.h"
#include "VolumeCutter.h"
#include "VolumeCutterGPU.h"

class vtkBoxWidgetCallback : public vtkCommand
{
public:
    static vtkBoxWidgetCallback * New()
    {
        return new vtkBoxWidgetCallback;
    }

    void Execute(vtkObject * caller, unsigned long, void *) override
    {
        vtkBoxWidget2 * boxWidget = reinterpret_cast<vtkBoxWidget2 *>(caller);
        vtkBoxRepresentation * rep = reinterpret_cast<vtkBoxRepresentation *>(boxWidget->GetRepresentation());
        vtkNew<vtkPlanes> planes;
        rep->GetPlanes(planes);
        rep->SetInsideOut(1);
        m_Mapper->SetClippingPlanes(planes);
    }
    vtkBoxWidgetCallback()
    {
    }

    vtkAbstractVolumeMapper * m_Mapper = nullptr;
};

vtkSmartPointer<vtkVolume> CreateVolume(vtkSmartPointer<vtkNrrdReader> reader);

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);

    QWidget wid;
    QHBoxLayout layMain;
    QVtkWidget * widget = new QVtkWidget();
    layMain.addWidget(widget);
    wid.setLayout(&layMain);
    wid.resize(600, 600);
    wid.setWindowTitle("Volume Cutting Demo");
    wid.show();

    vtkNew<vtkNrrdReader> volReader;
    volReader->SetFileName("D:/Hip.nrrd");
    volReader->Update();

    vtkSmartPointer<vtkVolume> volume = CreateVolume(volReader);

    widget->GetRenderer()->SetBackground(.03, .03, .03);
    widget->GetRenderer()->AddActor(volume);

    vtkNew<vtkBoxWidget2> boxWidget;
    boxWidget->SetInteractor(widget->GetRenderWindow()->GetInteractor());
    vtkNew<vtkBoxRepresentation> boxRep;
    boxWidget->SetRepresentation(boxRep);
    boxRep->SetPlaceFactor(1.0);
    boxRep->PlaceWidget(volume->GetBounds());
    vtkNew<vtkBoxWidgetCallback> callback;
    callback->m_Mapper = volume->GetMapper();
    boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);

    widget->SetBoxWidget2(boxWidget);

    QObject::connect(widget, &QVtkWidget::SgnCutActionTriggered, [&](bool inside, bool useGpu, const QImage & mask) {
        vtkGPUVolumeRayCastMapper * mapper = vtkGPUVolumeRayCastMapper::SafeDownCast(volume->GetMapper());
        vtkImageData * imageData = vtkImageData::SafeDownCast(mapper->GetInput());
        vtkBoxRepresentation * rep = vtkBoxRepresentation::SafeDownCast(boxWidget->GetRepresentation());
        vtkNew<vtkPlanes> planes;
        rep->SetInsideOut(0);
        rep->GetPlanes(planes);
        if (useGpu) {
            VolumeCutterGPU::Cut(imageData, widget->GetRenderer(), widget->GetRenderWindow(), mask, inside, planes);
        } else {
            VolumeCutter::Cut(imageData, widget->GetRenderer(), mask, inside, planes);
        }
        widget->GetRenderWindow()->Render();
    });

    return QApplication::exec();
}

vtkSmartPointer<vtkVolume> CreateVolume(vtkSmartPointer<vtkNrrdReader> reader)
{
    vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();

    // std430 最小只支持 4字节对齐
    vtkNew<vtkImageCast> castFilter;
    castFilter->SetInputData(reader->GetOutput());
    castFilter->SetOutputScalarTypeToInt();
    castFilter->Update();

    vtkNew<vtkGPUVolumeRayCastMapper> volMapper;
    volMapper->SetInputData(castFilter->GetOutput());
    volMapper->SetSampleDistance(0.5);

    vtkNew<vtkColorTransferFunction> colorFunc;
    colorFunc->AddRGBPoint(-1000.0, 0.0, 0.0, 0.0);
    colorFunc->AddRGBPoint(100.0, 0.85, 0.55, 0.35);
    colorFunc->AddRGBPoint(300.0, 0.95, 0.75, 0.55);
    colorFunc->AddRGBPoint(1000.0, 1.0, 1.0, 0.9);
    colorFunc->AddRGBPoint(3000.0, 1.0, 1.0, 1.0);

    vtkNew<vtkPiecewiseFunction> opacityFunc;
    opacityFunc->AddPoint(-1000.0, 0.0);
    opacityFunc->AddPoint(0.0, 0.0);
    opacityFunc->AddPoint(150.0, 0.05);
    opacityFunc->AddPoint(300.0, 0.1);
    opacityFunc->AddPoint(800.0, 0.3);
    opacityFunc->AddPoint(1200.0, 0.5);
    opacityFunc->AddPoint(3000.0, 0.9);

    vtkNew<vtkPiecewiseFunction> gradientFunc;
    gradientFunc->AddPoint(0.0, 0.0);
    gradientFunc->AddPoint(100.0, 0.2);
    gradientFunc->AddPoint(500.0, 0.8);

    vtkNew<vtkVolumeProperty> vrProperty;
    vrProperty->SetColor(colorFunc);
    vrProperty->SetScalarOpacity(opacityFunc);
    vrProperty->SetGradientOpacity(gradientFunc);
    vrProperty->SetInterpolationTypeToLinear();
    vrProperty->DisableGradientOpacityOn();
    vrProperty->ShadeOn();
    vrProperty->SetAmbient(0.3);
    vrProperty->SetDiffuse(0.8);
    vrProperty->SetSpecular(0.5);
    vrProperty->SetSpecularPower(20.0);

    volume->SetMapper(volMapper);
    volume->SetProperty(vrProperty);

    return volume;
}
