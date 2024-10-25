#include "S3DRenderer.h"

#include "S3DGridMapper.h"
#include "S3DRenderPass.h"

#include <vtkAbstractArray.h>
#include <vtkActor.h>
#include <vtkActor2DCollection.h>
#include <vtkAxesActor.h>
#include <vtkCameraOrientationWidget.h>
#include <vtkBoundingBox.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <vtkFieldData.h>
#include <vtkImporter.h>
#include <vtkLightsPass.h>
#include <vtkObjectFactory.h>
#include <vtkOpenGLFXAAPass.h>
#include <vtkOpenGLRenderer.h>
#include <vtkOpenGLTexture.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderPassCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRendererCollection.h>
#include <vtkToneMappingPass.h>
#include <vtksys/Directory.hxx>
#include <vtksys/SystemTools.hxx>

#include <vtk_glew.h>

#include <chrono>
#include <cmath>

#include <QDebug>

vtkStandardNewMacro(S3DRenderer);

void S3DRenderer::Initialize()
{
    RemoveAllViewProps();
    RemoveAllLights();
    UseSSAOPass = false;
    UseToneMappingPass = false;
    SetBackground(0.2, 0.2, 0.2);
    AutomaticLightCreationOn();
    SetupRenderPasses();
}

void S3DRenderer::SetupRenderPasses()
{
    vtkRenderPass * pass = GetPass();
    if (pass) {
        pass->ReleaseGraphicsResources(RenderWindow);
    }
    vtkNew<S3DRenderPass> newPass;
    newPass->SetUseSSAOPass(UseSSAOPass);
    double bounds[6];
    ComputeVisiblePropBounds(bounds);
    newPass->SetBounds(bounds);
    vtkSmartPointer<vtkRenderPass> renderingPass = newPass;
    if (UseToneMappingPass) {
        vtkNew<vtkToneMappingPass> toneP;
        toneP->SetToneMappingType(vtkToneMappingPass::GenericFilmic);
        toneP->SetGenericFilmicDefaultPresets();
        toneP->SetDelegatePass(renderingPass);
        renderingPass = toneP;
    }
    SetPass(renderingPass);
}

void S3DRenderer::InitializeCamera()
{
    static bool firstTime = true;
    if (firstTime) {
        vtkCamera * cam = GetActiveCamera();
        cam->SetPosition(1, 0, 0);
        cam->SetViewUp(0, 0, 1);
        cam->OrthogonalizeViewUp();
        cam->ComputeViewPlaneNormal();
        firstTime = false;
    }
    Superclass::ResetCamera();
}

void S3DRenderer::ShowAxis(bool show)
{
    if (show) {
        vtkNew<vtkAxesActor> axes;
        AxisWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
        AxisWidget->SetOrientationMarker(axes);
        AxisWidget->SetInteractor(RenderWindow->GetInteractor());
        AxisWidget->SetViewport(0.85, 0.0, 1.0, 0.15);
        AxisWidget->On();
        AxisWidget->InteractiveOff();
        AxisWidget->SetKeyPressActivation(false);
    } else {
        AxisWidget = nullptr;
    }
    SetupRenderPasses();
}

void S3DRenderer::ShowCameraOrientation(bool show)
{
    if (show) {
        CameraOrientationWidget = vtkSmartPointer<vtkCameraOrientationWidget>::New();
        CameraOrientationWidget->SetParentRenderer(this);
        CameraOrientationWidget->On();
        CameraOrientationWidget->SetKeyPressActivation(false);
    } else {
        CameraOrientationWidget = nullptr;
    }
    SetupRenderPasses();
}

void S3DRenderer::ShowGrid(bool show)
{
    double bounds[6];
    ComputeVisiblePropBounds(bounds);

    vtkBoundingBox bbox(bounds);

    if (bbox.IsValid()) {
        SetClippingRangeExpansion(0.99);

        double diag = bbox.GetDiagonalLength();
        double unitSquare = pow(10.0, round(log10(diag * 0.1)));

        double gridX = 0.5 * (bounds[0] + bounds[1]);
        double gridY = 0.0 * (bounds[2] + bounds[3]);
        double gridZ = bounds[4];

        vtkNew<S3DGridMapper> gridMapper;
        gridMapper->SetFadeDistance(diag);
        gridMapper->SetUnitSquare(unitSquare);

        GridActor->GetProperty()->SetColor(0.0, 0.0, 0.0);
        GridActor->ForceTranslucentOn();
        GridActor->SetPosition(gridX, gridY, gridZ);
        GridActor->SetMapper(gridMapper);
        GridActor->UseBoundsOff();

        RemoveActor(GridActor);
        AddActor(GridActor);
    } else {
        SetClippingRangeExpansion(0);
        show = false;
    }

    GridActor->SetVisibility(show);
    ResetCameraClippingRange();
    SetupRenderPasses();
}

void S3DRenderer::ShowEdge(bool show)
{
    vtkActor * anActor;
    vtkActorCollection * ac = GetActors();
    vtkCollectionSimpleIterator ait;
    for (ac->InitTraversal(ait); (anActor = ac->GetNextActor(ait));) {
        anActor->GetProperty()->SetEdgeVisibility(show);
    }
}

void S3DRenderer::SetUseSSAOPass(bool use)
{
    UseSSAOPass = use;
    SetupRenderPasses();
}

void S3DRenderer::SetUseToneMappingPass(bool use)
{
    UseToneMappingPass = use;
    SetupRenderPasses();
}
