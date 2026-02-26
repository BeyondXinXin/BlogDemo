#ifndef vtkInteractorStyleFixedCenter_H
#define vtkInteractorStyleFixedCenter_H

#include <vtkInteractorStyleTrackballCamera.h>

// 指定中心模式
class vtkInteractorStyleFixedCenter : public vtkInteractorStyleTrackballCamera
{
public:
    static vtkInteractorStyleFixedCenter * New();
    vtkTypeMacro(vtkInteractorStyleFixedCenter, vtkInteractorStyleTrackballCamera);

    void Rotate() override;
    void Pan() override;
    void Dolly() override;

    vtkSetVector3Macro(FixedCenter, double);
    vtkGetVector3Macro(FixedCenter, double);

protected:
    vtkInteractorStyleFixedCenter();
    ~vtkInteractorStyleFixedCenter() override = default;

    void Dolly(double factor);

private:
    double FixedCenter[3];
};

#endif // vtkInteractorStyleFixedCenter_H