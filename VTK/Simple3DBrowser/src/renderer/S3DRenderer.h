#ifndef vtkS3DRenderer_h
#define vtkS3DRenderer_h

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCornerAnnotation.h>
#include <vtkMapper.h>
#include <vtkNew.h>
#include <vtkOpenGLRenderer.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkCameraOrientationWidget.h>

class S3DRenderer : public vtkOpenGLRenderer
{
public:
    static S3DRenderer * New();
    vtkTypeMacro(S3DRenderer, vtkOpenGLRenderer);

    void Initialize();
    void SetupRenderPasses();
    void InitializeCamera();

    void ShowAxis(bool show);
    void ShowCameraOrientation(bool show);
    void ShowGrid(bool show);
    void ShowEdge(bool show);

    void SetUseSSAOPass(bool use);
    void SetUseToneMappingPass(bool use);

private:
    S3DRenderer() = default;
    ~S3DRenderer() override = default;

private:
    vtkNew<vtkActor> GridActor;
    vtkSmartPointer<vtkOrientationMarkerWidget> AxisWidget;
    vtkSmartPointer<vtkCameraOrientationWidget> CameraOrientationWidget;

    bool UseSSAOPass;
    bool UseToneMappingPass;
};

#endif
