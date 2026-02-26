#include "vtkInteractorStyleFixedCenter.h"

#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkMath.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkTransform.h>
#include <vtkNew.h>

vtkStandardNewMacro(vtkInteractorStyleFixedCenter);

vtkInteractorStyleFixedCenter::vtkInteractorStyleFixedCenter()
{
    FixedCenter[0] = 0.0;
    FixedCenter[1] = 0.0;
    FixedCenter[2] = 0.0;
}

void vtkInteractorStyleFixedCenter::Rotate()
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
    transform->Translate(FixedCenter[0], FixedCenter[1], FixedCenter[2]);
    transform->RotateWXYZ(ryf, rightAxis);
    transform->RotateWXYZ(rxf, viewUp);
    transform->Translate(-FixedCenter[0], -FixedCenter[1], -FixedCenter[2]);

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

void vtkInteractorStyleFixedCenter::Pan()
{
    if (CurrentRenderer == nullptr) {
        return;
    }

    vtkRenderWindowInteractor * rwi = Interactor;

    double viewFocus[4], focalDepth, viewPoint[3];
    double newPickPoint[4], oldPickPoint[4], motionVector[3];

    vtkCamera * camera = CurrentRenderer->GetActiveCamera();
    camera->GetFocalPoint(viewFocus);
    ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2], viewFocus);
    focalDepth = viewFocus[2];

    ComputeDisplayToWorld(
      rwi->GetEventPosition()[0], rwi->GetEventPosition()[1], focalDepth, newPickPoint);

    ComputeDisplayToWorld(
      rwi->GetLastEventPosition()[0], rwi->GetLastEventPosition()[1], focalDepth, oldPickPoint);

    motionVector[0] = oldPickPoint[0] - newPickPoint[0];
    motionVector[1] = oldPickPoint[1] - newPickPoint[1];
    motionVector[2] = oldPickPoint[2] - newPickPoint[2];

    camera->GetFocalPoint(viewFocus);
    camera->GetPosition(viewPoint);
    camera->SetFocalPoint(
      motionVector[0] + viewFocus[0], motionVector[1] + viewFocus[1], motionVector[2] + viewFocus[2]);

    camera->SetPosition(
      motionVector[0] + viewPoint[0], motionVector[1] + viewPoint[1], motionVector[2] + viewPoint[2]);

    if (rwi->GetLightFollowCamera()) {
        CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }

    rwi->Render();
}

void vtkInteractorStyleFixedCenter::Dolly()
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

void vtkInteractorStyleFixedCenter::Dolly(double factor)
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
            position[i] = FixedCenter[i] + (position[i] - FixedCenter[i]) / factor;
            focalPoint[i] = FixedCenter[i] + (focalPoint[i] - FixedCenter[i]) / factor;
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
