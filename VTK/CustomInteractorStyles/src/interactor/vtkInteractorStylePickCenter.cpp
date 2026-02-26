#include "vtkInteractorStylePickCenter.h"

#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkMath.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkTransform.h>
#include <vtkNew.h>

vtkStandardNewMacro(vtkInteractorStylePickCenter);

vtkInteractorStylePickCenter::vtkInteractorStylePickCenter()
{
    PickedCenter[0] = 0.0;
    PickedCenter[1] = 0.0;
    PickedCenter[2] = 0.0;
}

void vtkInteractorStylePickCenter::ComputePickedCenter()
{
    if (CurrentRenderer == nullptr) {
        return;
    }

    vtkRenderWindowInteractor * rwi = Interactor;
    vtkCamera * camera = CurrentRenderer->GetActiveCamera();

    double viewFocus[4];
    camera->GetFocalPoint(viewFocus);
    ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2], viewFocus);
    double focalDepth = viewFocus[2];

    double worldPos[4];
    ComputeDisplayToWorld(
      rwi->GetEventPosition()[0], rwi->GetEventPosition()[1], focalDepth, worldPos);

    PickedCenter[0] = worldPos[0];
    PickedCenter[1] = worldPos[1];
    PickedCenter[2] = worldPos[2];
}

void vtkInteractorStylePickCenter::OnLeftButtonDown()
{
    FindPokedRenderer(Interactor->GetEventPosition()[0], Interactor->GetEventPosition()[1]);
    ComputePickedCenter();
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

void vtkInteractorStylePickCenter::OnMiddleButtonDown()
{
    FindPokedRenderer(Interactor->GetEventPosition()[0], Interactor->GetEventPosition()[1]);
    ComputePickedCenter();
    vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();
}

void vtkInteractorStylePickCenter::OnRightButtonDown()
{
    FindPokedRenderer(Interactor->GetEventPosition()[0], Interactor->GetEventPosition()[1]);
    ComputePickedCenter();
    vtkInteractorStyleTrackballCamera::OnRightButtonDown();
}

void vtkInteractorStylePickCenter::Rotate()
{
    if (CurrentRenderer == nullptr) {
        return;
    }

    vtkRenderWindowInteractor * rwi = Interactor;

    int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
    int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
    dy = -dy;

    const int * size = CurrentRenderer->GetRenderWindow()->GetSize();

    double delta_elevation = -20.0 / size[1];
    double delta_azimuth = -20.0 / size[0];

    double rxf = dx * delta_azimuth * MotionFactor;
    double ryf = dy * delta_elevation * MotionFactor;

    vtkCamera * camera = CurrentRenderer->GetActiveCamera();

    double position[3], focalPoint[3], viewUp[3];
    camera->GetPosition(position);
    camera->GetFocalPoint(focalPoint);
    camera->GetViewUp(viewUp);

    double dop[3];
    vtkMath::Subtract(focalPoint, position, dop);
    vtkMath::Normalize(dop);
    double rightAxis[3];
    vtkMath::Cross(dop, viewUp, rightAxis);
    vtkMath::Normalize(rightAxis);

    vtkNew<vtkTransform> transform;
    transform->Translate(PickedCenter[0], PickedCenter[1], PickedCenter[2]);
    transform->RotateWXYZ(ryf, rightAxis);
    transform->RotateWXYZ(rxf, viewUp);
    transform->Translate(-PickedCenter[0], -PickedCenter[1], -PickedCenter[2]);

    double newPosition[3], newFocalPoint[3], newViewUp[3];
    transform->TransformPoint(position, newPosition);
    transform->TransformPoint(focalPoint, newFocalPoint);
    transform->TransformVector(viewUp, newViewUp);

    camera->SetPosition(newPosition);
    camera->SetFocalPoint(newFocalPoint);
    camera->SetViewUp(newViewUp);
    camera->OrthogonalizeViewUp();

    if (AutoAdjustCameraClippingRange) {
        CurrentRenderer->ResetCameraClippingRange();
    }

    if (rwi->GetLightFollowCamera()) {
        CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }

    rwi->Render();
}

void vtkInteractorStylePickCenter::Dolly()
{
    if (CurrentRenderer == nullptr) {
        return;
    }

    vtkRenderWindowInteractor * rwi = Interactor;
    double * center = CurrentRenderer->GetCenter();
    int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
    double dyf = MotionFactor * dy / center[1];
    Dolly(pow(1.1, dyf));
}

void vtkInteractorStylePickCenter::Dolly(double factor)
{
    if (CurrentRenderer == nullptr) {
        return;
    }

    vtkCamera * camera = CurrentRenderer->GetActiveCamera();
    if (camera->GetParallelProjection()) {
        camera->SetParallelScale(camera->GetParallelScale() / factor);
    } else {
        double position[3], focalPoint[3];
        camera->GetPosition(position);
        camera->GetFocalPoint(focalPoint);

        for (int i = 0; i < 3; i++) {
            position[i] = PickedCenter[i] + (position[i] - PickedCenter[i]) / factor;
            focalPoint[i] = PickedCenter[i] + (focalPoint[i] - PickedCenter[i]) / factor;
        }

        camera->SetPosition(position);
        camera->SetFocalPoint(focalPoint);

        if (AutoAdjustCameraClippingRange) {
            CurrentRenderer->ResetCameraClippingRange();
        }
    }

    if (Interactor->GetLightFollowCamera()) {
        CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }

    Interactor->Render();
}
