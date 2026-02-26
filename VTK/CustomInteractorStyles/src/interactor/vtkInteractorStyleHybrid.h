#ifndef vtkInteractorStyleHybrid_H
#define vtkInteractorStyleHybrid_H

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkSmartPointer.h>

class vtkCellPicker;

// 模型+场景混合模式
class vtkInteractorStyleHybrid : public vtkInteractorStyleTrackballCamera
{
public:
    static vtkInteractorStyleHybrid * New();
    vtkTypeMacro(vtkInteractorStyleHybrid, vtkInteractorStyleTrackballCamera);

    void OnLeftButtonDown() override;
    void OnLeftButtonUp() override;
    void OnMiddleButtonDown() override;
    void OnMiddleButtonUp() override;
    void OnRightButtonDown() override;
    void OnRightButtonUp() override;
    void OnMouseMove() override;

    void SetInteractor(vtkRenderWindowInteractor * interactor) override;

protected:
    vtkInteractorStyleHybrid();
    ~vtkInteractorStyleHybrid() override = default;

    bool PickActor(int x, int y);

private:
    vtkSmartPointer<vtkInteractorStyleTrackballActor> ActorStyle;
    vtkSmartPointer<vtkCellPicker> Picker;

    bool InteractingWithActor = false;
};

#endif // vtkInteractorStyleHybrid_H