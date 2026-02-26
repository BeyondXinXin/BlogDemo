#ifndef vtkInteractorStyleSingleModel_H
#define vtkInteractorStyleSingleModel_H

#include <vtkInteractorStyleTrackballActor.h>

// 模型模式，vtk默认
class vtkInteractorStyleSingleModel : public vtkInteractorStyleTrackballActor
{
public:
    static vtkInteractorStyleSingleModel * New();
    vtkTypeMacro(vtkInteractorStyleSingleModel, vtkInteractorStyleTrackballActor);

protected:
    vtkInteractorStyleSingleModel();
    ~vtkInteractorStyleSingleModel() override = default;
};

#endif // vtkInteractorStyleSingleModel_H