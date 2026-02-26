#include "vtkInteractorStyleSelectionOnly.h"

#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#include <vtkCellPicker.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkProperty.h>

#include <cmath>

vtkStandardNewMacro(vtkInteractorStyleSelectionOnly);

vtkInteractorStyleSelectionOnly::vtkInteractorStyleSelectionOnly()
{
    Picker = vtkSmartPointer<vtkCellPicker>::New();
    Picker->SetTolerance(0.005);
}

void vtkInteractorStyleSelectionOnly::OnLeftButtonDown()
{
    PressPosition[0] = Interactor->GetEventPosition()[0];
    PressPosition[1] = Interactor->GetEventPosition()[1];
}

void vtkInteractorStyleSelectionOnly::OnLeftButtonUp()
{
    int x = Interactor->GetEventPosition()[0];
    int y = Interactor->GetEventPosition()[1];

    double dist = std::sqrt(
      (x - PressPosition[0]) * (x - PressPosition[0]) + (y - PressPosition[1]) * (y - PressPosition[1]));
    if (dist > 5.0) {
        return;
    }

    FindPokedRenderer(x, y);
    if (CurrentRenderer == nullptr) {
        return;
    }

    Picker->Pick(x, y, 0.0, CurrentRenderer);
    vtkActor * actor = Picker->GetActor();

    if (actor != nullptr) {
        bool wasSelected = IsActorSelected(actor);
        SetActorSelected(actor, !wasSelected);
    }
}

void vtkInteractorStyleSelectionOnly::Rotate()
{ }

void vtkInteractorStyleSelectionOnly::Pan()
{ }

void vtkInteractorStyleSelectionOnly::Dolly()
{ }

void vtkInteractorStyleSelectionOnly::Spin()
{ }

void vtkInteractorStyleSelectionOnly::SetSelectionChangedCallback(SelectionChangedCallback cb)
{
    OnSelectionChanged = std::move(cb);
}

void vtkInteractorStyleSelectionOnly::SetActorSelected(vtkActor * actor, bool selected)
{
    if (actor == nullptr) {
        return;
    }

    if (selected) {
        SelectedActors.insert(actor);
    } else {
        SelectedActors.erase(actor);
    }

    HighlightActor(actor, selected);

    if (OnSelectionChanged) {
        OnSelectionChanged(actor, selected);
    }

    if (Interactor) {
        Interactor->Render();
    }
}

bool vtkInteractorStyleSelectionOnly::IsActorSelected(vtkActor * actor) const
{
    return SelectedActors.find(actor) != SelectedActors.end();
}

void vtkInteractorStyleSelectionOnly::ClearSelection()
{
    // 循环中会修改
    auto copy = SelectedActors;
    for (auto * actor : copy) {
        SetActorSelected(actor, false);
    }
}

void vtkInteractorStyleSelectionOnly::HighlightActor(vtkActor * actor, bool selected)
{
    if (actor == nullptr) {
        return;
    }

    if (selected) {
        actor->GetProperty()->EdgeVisibilityOn();
        actor->GetProperty()->SetEdgeColor(1.0, 1.0, 0.0);
        actor->GetProperty()->SetLineWidth(2.0);
    } else {
        actor->GetProperty()->EdgeVisibilityOff();
    }
}
