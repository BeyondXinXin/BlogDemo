#ifndef vtkInteractorStyleSelectionOnly_H
#define vtkInteractorStyleSelectionOnly_H

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSmartPointer.h>

#include <functional>
#include <set>

class vtkCellPicker;
class vtkActor;

// 选择模式
class vtkInteractorStyleSelectionOnly : public vtkInteractorStyleTrackballCamera
{
public:
    static vtkInteractorStyleSelectionOnly * New();
    vtkTypeMacro(vtkInteractorStyleSelectionOnly, vtkInteractorStyleTrackballCamera);

    void OnLeftButtonDown() override;
    void OnLeftButtonUp() override;

    void Rotate() override;
    void Pan() override;
    void Dolly() override;
    void Spin() override;

    using SelectionChangedCallback = std::function<void(vtkActor *, bool)>;
    void SetSelectionChangedCallback(SelectionChangedCallback cb);

    void SetActorSelected(vtkActor * actor, bool selected);

    bool IsActorSelected(vtkActor * actor) const;

    void ClearSelection();

protected:
    vtkInteractorStyleSelectionOnly();
    ~vtkInteractorStyleSelectionOnly() override = default;

    void HighlightActor(vtkActor * actor, bool selected);

private:
    vtkSmartPointer<vtkCellPicker> Picker;
    std::set<vtkActor *> SelectedActors;
    SelectionChangedCallback OnSelectionChanged;

    int PressPosition[2] = { 0, 0 };
};

#endif // vtkInteractorStyleSelectionOnly_H