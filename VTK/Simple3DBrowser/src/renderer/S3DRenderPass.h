#ifndef S3DRenderPass_h
#define S3DRenderPass_h

#include <vtkFramebufferPass.h>
#include <vtkOpenGLQuadHelper.h>
#include <vtkSmartPointer.h>
#include <vtkTimeStamp.h>

#include <memory>
#include <vector>

class vtkProp;

class S3DRenderPass : public vtkRenderPass
{
public:
    static S3DRenderPass * New();
    vtkTypeMacro(S3DRenderPass, vtkRenderPass);

    vtkGetMacro(UseRaytracing, bool);
    vtkSetMacro(UseRaytracing, bool);
    vtkBooleanMacro(UseRaytracing, bool);

    vtkGetMacro(UseSSAOPass, bool);
    vtkSetMacro(UseSSAOPass, bool);
    vtkBooleanMacro(UseSSAOPass, bool);

    vtkGetMacro(UseDepthPeelingPass, bool);
    vtkSetMacro(UseDepthPeelingPass, bool);
    vtkBooleanMacro(UseDepthPeelingPass, bool);

    vtkGetMacro(UseBlurBackground, bool);
    vtkSetMacro(UseBlurBackground, bool);
    vtkBooleanMacro(UseBlurBackground, bool);

    vtkGetVector6Macro(Bounds, double);
    vtkSetVector6Macro(Bounds, double);

    void Render(const vtkRenderState * s) override;

private:
    S3DRenderPass() = default;
    ~S3DRenderPass() override = default;
    S3DRenderPass(const S3DRenderPass &) = delete;
    void operator=(const S3DRenderPass &) = delete;

    void ReleaseGraphicsResources(vtkWindow * w) override;
    void Initialize(const vtkRenderState * s);
    void Blend(const vtkRenderState * s);

private:
    bool UseRaytracing = false;
    bool UseSSAOPass = false;
    bool UseDepthPeelingPass = false;
    bool UseBlurBackground = true;

    vtkSmartPointer<vtkFramebufferPass> BackgroundPass;
    vtkSmartPointer<vtkFramebufferPass> MainPass;

    double Bounds[6];

    vtkMTimeType InitializeTime = 0;

    std::vector<vtkProp *> BackgroundProps;
    std::vector<vtkProp *> MainProps;

    std::shared_ptr<vtkOpenGLQuadHelper> BlendQuadHelper;
};

#endif
