#ifndef vtkInteractorStyleSceneCamera_H
#define vtkInteractorStyleSceneCamera_H

#include <vtkInteractorStyleTrackballCamera.h>

// 相机模式，vtk默认
class vtkInteractorStyleSceneCamera : public vtkInteractorStyleTrackballCamera
{
public:
    static vtkInteractorStyleSceneCamera * New();
    vtkTypeMacro(vtkInteractorStyleSceneCamera, vtkInteractorStyleTrackballCamera);

protected:
    vtkInteractorStyleSceneCamera();
    ~vtkInteractorStyleSceneCamera() override = default;
};

#endif // vtkInteractorStyleSceneCamera_H