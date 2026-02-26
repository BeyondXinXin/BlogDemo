#ifndef vtkInteractorStylePickCenter_H
#define vtkInteractorStylePickCenter_H

#include <vtkInteractorStyleTrackballCamera.h>

// 鼠标点中心模式
class vtkInteractorStylePickCenter : public vtkInteractorStyleTrackballCamera
{
public:
    static vtkInteractorStylePickCenter * New();
    vtkTypeMacro(vtkInteractorStylePickCenter, vtkInteractorStyleTrackballCamera);

    void OnLeftButtonDown() override;
    void OnMiddleButtonDown() override;
    void OnRightButtonDown() override;

    void Rotate() override;
    void Dolly() override;

protected:
    vtkInteractorStylePickCenter();
    ~vtkInteractorStylePickCenter() override = default;

    void Dolly(double factor);

    void ComputePickedCenter();

private:
    double PickedCenter[3];
};

#endif // vtkInteractorStylePickCenter_H