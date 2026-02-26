#include "vtkInteractorStyleHybrid.h"

#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#include <vtkCellPicker.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkNew.h>

vtkStandardNewMacro(vtkInteractorStyleHybrid);

vtkInteractorStyleHybrid::vtkInteractorStyleHybrid()
{
    Picker = vtkSmartPointer<vtkCellPicker>::New();
    Picker->SetTolerance(0.005);

    ActorStyle = vtkSmartPointer<vtkInteractorStyleTrackballActor>::New();
}

void vtkInteractorStyleHybrid::SetInteractor(vtkRenderWindowInteractor * interactor)
{
    ActorStyle->SetInteractor(nullptr);
    InteractingWithActor = false;
    vtkInteractorStyleTrackballCamera::SetInteractor(interactor);
}

bool vtkInteractorStyleHybrid::PickActor(int x, int y)
{
    FindPokedRenderer(x, y);
    if (CurrentRenderer == nullptr) {
        return false;
    }

    Picker->Pick(x, y, 0.0, CurrentRenderer);
    vtkActor * actor = Picker->GetActor();
    return (actor != nullptr);
}

void vtkInteractorStyleHybrid::OnLeftButtonDown()
{
    int x = Interactor->GetEventPosition()[0];
    int y = Interactor->GetEventPosition()[1];

    InteractingWithActor = PickActor(x, y);

    if (InteractingWithActor) {
        ActorStyle->SetInteractor(Interactor);
        ActorStyle->SetCurrentRenderer(CurrentRenderer);
        ActorStyle->OnLeftButtonDown();
    } else {
        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }
}

void vtkInteractorStyleHybrid::OnLeftButtonUp()
{
    if (InteractingWithActor) {
        ActorStyle->OnLeftButtonUp();
        InteractingWithActor = false;
    } else {
        vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
    }
}

void vtkInteractorStyleHybrid::OnMiddleButtonDown()
{
    int x = Interactor->GetEventPosition()[0];
    int y = Interactor->GetEventPosition()[1];

    InteractingWithActor = PickActor(x, y);

    if (InteractingWithActor) {
        ActorStyle->SetInteractor(Interactor);
        ActorStyle->SetCurrentRenderer(CurrentRenderer);
        ActorStyle->OnMiddleButtonDown();
    } else {
        vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();
    }
}

void vtkInteractorStyleHybrid::OnMiddleButtonUp()
{
    if (InteractingWithActor) {
        ActorStyle->OnMiddleButtonUp();
        InteractingWithActor = false;
    } else {
        vtkInteractorStyleTrackballCamera::OnMiddleButtonUp();
    }
}

void vtkInteractorStyleHybrid::OnRightButtonDown()
{
    int x = Interactor->GetEventPosition()[0];
    int y = Interactor->GetEventPosition()[1];

    InteractingWithActor = PickActor(x, y);

    if (InteractingWithActor) {
        ActorStyle->SetInteractor(Interactor);
        ActorStyle->SetCurrentRenderer(CurrentRenderer);
        ActorStyle->OnRightButtonDown();
    } else {
        vtkInteractorStyleTrackballCamera::OnRightButtonDown();
    }
}

void vtkInteractorStyleHybrid::OnRightButtonUp()
{
    if (InteractingWithActor) {
        ActorStyle->OnRightButtonUp();
        InteractingWithActor = false;
    } else {
        vtkInteractorStyleTrackballCamera::OnRightButtonUp();
    }
}

void vtkInteractorStyleHybrid::OnMouseMove()
{
    if (InteractingWithActor) {
        ActorStyle->OnMouseMove();
    } else {
        vtkInteractorStyleTrackballCamera::OnMouseMove();
    }
}
