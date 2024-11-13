#include "S3DRenderPass.h"

#include <vtkBoundingBox.h>
#include <vtkCameraPass.h>
#include <vtkDualDepthPeelingPass.h>
#include <vtkLightsPass.h>
#include <vtkObjectFactory.h>
#include <vtkOpaquePass.h>
#include <vtkOpenGLFXAAPass.h>
#include <vtkOpenGLRenderUtilities.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkOpenGLShaderCache.h>
#include <vtkOpenGLState.h>
#include <vtkOverlayPass.h>
#include <vtkProp.h>
#include <vtkRenderPassCollection.h>
#include <vtkRenderState.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSSAOPass.h>
#include <vtkSequencePass.h>
#include <vtkShaderProgram.h>
#include <vtkSkybox.h>
#include <vtkTextureObject.h>
#include <vtkToneMappingPass.h>
#include <vtkTranslucentPass.h>
#include <vtkVolumetricPass.h>

#include <sstream>

vtkStandardNewMacro(S3DRenderPass);

/**
 * @brief 渲染当前场景，将背景和主内容进行渲染，并通过混合 Pass 完成最终图像的输出。
 *
 * @param s 当前的渲染状态，包含渲染器、对象列表和帧缓冲信息。
 */
void S3DRenderPass::Render(const vtkRenderState * s)
{
    // 初始化渲染状态
    Initialize(s);
    // 渲染背景对象
    vtkRenderState backgroundState(s->GetRenderer());
    backgroundState.SetPropArrayAndCount(
      BackgroundProps.data(), static_cast<int>(BackgroundProps.size()));
    backgroundState.SetFrameBuffer(s->GetFrameBuffer());
    BackgroundPass->Render(&backgroundState);

    // 渲染主对象
    vtkRenderState mainState(s->GetRenderer());
    mainState.SetPropArrayAndCount(MainProps.data(), static_cast<int>(MainProps.size()));
    mainState.SetFrameBuffer(s->GetFrameBuffer());
    MainPass->Render(&mainState);

    // 执行背景和主内容的混合
    Blend(s);

    // 更新渲染的对象数量
    NumberOfRenderedProps = MainPass->GetNumberOfRenderedProps();
}

/**
 * @brief 释放图形资源，防止 GPU 资源泄漏。
 *
 * 当渲染窗口被销毁或不再使用时，需要释放与之关联的 GPU 资源，确保内存得到及时回收。
 * 此方法依次释放混合四边形帮助器、背景 Pass 和主 Pass 中的资源。
 *
 * @param w 当前的渲染窗口，用于识别需要释放资源的上下文。
 */
void S3DRenderPass::ReleaseGraphicsResources(vtkWindow * w)
{
    if (BlendQuadHelper) { // 混合四边形帮助器的资源
        BlendQuadHelper->ReleaseGraphicsResources(w);
    }

    if (BackgroundPass) { // 背景 Pass
        BackgroundPass->ReleaseGraphicsResources(w);
    }

    if (MainPass) { // 主 Pass
        MainPass->ReleaseGraphicsResources(w);
    }
}

/**
 * @brief 初始化渲染过程中的各种 Pass，并设置帧缓冲区与相关渲染属性。
 *
 * 该函数根据场景中的对象分类并初始化不同的渲染 Pass，确保渲染管线的顺利运行。
 * 渲染流程分为背景渲染、主渲染和后处理。根据是否使用 SSAO（屏幕空间环境光遮蔽）和深度剥离
 * （Depth Peeling）来调整渲染顺序。帧缓冲区用于存储中间渲染结果以进行进一步处理。
 *
 * @param s 当前的渲染状态，用于获取场景中的对象和渲染器。
 */
void S3DRenderPass::Initialize(const vtkRenderState * s)
{
    // 检查是否需要重新初始化，如果已经初始化则直接返回
    if (InitializeTime == MTime) {
        return;
    }

    // 释放图形资源并清空对象列表
    ReleaseGraphicsResources(s->GetRenderer()->GetRenderWindow());
    BackgroundProps.clear();
    MainProps.clear();

    {
        // 分类场景中的对象为背景或主对象
        vtkProp ** props = s->GetPropArray();
        for (int i = 0; i < s->GetPropArrayCount(); i++) {
            vtkProp * prop = props[i];
            if (vtkSkybox::SafeDownCast(prop)) {
                BackgroundProps.push_back(prop); // 添加到背景对象列表
            } else {
                MainProps.push_back(prop); // 添加到主对象列表
            }
        }
    }

    {
        // 初始化背景帧缓冲 Pass
        vtkNew<vtkOpaquePass> bgP;
        vtkNew<vtkCameraPass> camP;
        camP->SetDelegatePass(bgP);
        BackgroundPass = vtkSmartPointer<vtkFramebufferPass>::New();
        BackgroundPass->SetDelegatePass(camP);
        BackgroundPass->SetColorFormat(vtkTextureObject::Float32);
    }

    {
        // 初始化主渲染 Pass 集合
        vtkNew<vtkLightsPass> lightsP;
        vtkNew<vtkOpaquePass> opaqueP;
        vtkNew<vtkTranslucentPass> translucentP;
        vtkNew<vtkVolumetricPass> volumeP;
        vtkNew<vtkOverlayPass> overlayP;
        vtkNew<vtkRenderPassCollection> collection;
        collection->AddItem(lightsP);

        // 根据设置选择 SSAO 或不透明 Pass
        if (UseSSAOPass) {
            vtkBoundingBox bbox(Bounds);
            if (bbox.IsValid()) {
                vtkNew<vtkCameraPass> ssaoCamP;
                ssaoCamP->SetDelegatePass(opaqueP);

                vtkNew<vtkSSAOPass> ssaoP;
                ssaoP->SetRadius(0.1 * bbox.GetDiagonalLength());
                ssaoP->SetBias(0.001 * bbox.GetDiagonalLength());
                ssaoP->SetKernelSize(200);
                ssaoP->SetDelegatePass(ssaoCamP);

                collection->AddItem(ssaoP);
            } else {
                collection->AddItem(opaqueP);
            }
        } else {
            collection->AddItem(opaqueP);
        }

        // 添加透明和体积 Pass，根据是否启用深度剥离进行选择
        if (UseDepthPeelingPass) {
            vtkNew<vtkDualDepthPeelingPass> ddpP;
            ddpP->SetTranslucentPass(translucentP);
            ddpP->SetVolumetricPass(volumeP);
            collection->AddItem(ddpP);
        } else {
            collection->AddItem(translucentP);
            collection->AddItem(volumeP);
        }
        collection->AddItem(overlayP);

        // 创建顺序 Pass，并将其委托给相机 Pass
        vtkNew<vtkSequencePass> sequence;
        sequence->SetPasses(collection);
        vtkNew<vtkCameraPass> camP;
        camP->SetDelegatePass(sequence);

        // 初始化主帧缓冲 Pass
        MainPass = vtkSmartPointer<vtkFramebufferPass>::New();
        MainPass->SetDelegatePass(camP);
        MainPass->SetColorFormat(vtkTextureObject::Float32);
    }

    // 更新初始化时间戳
    InitializeTime = GetMTime();
}

/**
 * @brief 将背景和主渲染结果进行混合，应用模糊和透明度效果。
 *
 * 此方法通过全屏四边形着色器，将主渲染和背景帧缓冲区的纹理混合，实现背景模糊和前景透明度的融合。
 *
 * @param s 当前的渲染状态，包含渲染器和窗口信息。
 */
void S3DRenderPass::Blend(const vtkRenderState * s)
{
    // 获取渲染器和 OpenGL 状态
    vtkRenderer * r = s->GetRenderer();
    vtkOpenGLRenderWindow * renWin = static_cast<vtkOpenGLRenderWindow *>(r->GetRenderWindow());
    vtkOpenGLState * ostate = renWin->GetState();

    // 清除渲染器，并禁用深度测试和混合
    r->Clear();
    vtkOpenGLState::ScopedglEnableDisable bsaver(ostate, GL_BLEND);
    vtkOpenGLState::ScopedglEnableDisable dsaver(ostate, GL_DEPTH_TEST);
    ostate->vtkglDisable(GL_DEPTH_TEST);
    ostate->vtkglDisable(GL_BLEND);

    // 检查或更新全屏四边形帮助器的着色器
    if (BlendQuadHelper && BlendQuadHelper->ShaderChangeValue < GetMTime()) {
        BlendQuadHelper = nullptr;
    }

    if (!BlendQuadHelper) {
        // 构建片段着色器源代码
        std::string FSSource = vtkOpenGLRenderUtilities::GetFullScreenQuadFragmentShaderTemplate();

        std::stringstream ssDecl;
        ssDecl << "uniform sampler2D texBackground;\n"
                  "uniform sampler2D texMain;\n"
                  "#define SAMPLES 200\n"
                  "#define GOLDEN_ANGLE 2.399963\n"
                  "vec3 BokehBlur(vec2 tcoords, float radius) {\n"
                  "  vec3 acc = vec3(0.0);\n"
                  "  float factor = radius / sqrt(float(SAMPLES));\n"
                  "  for (int i = 0; i < SAMPLES; i++) {\n"
                  "    float theta = float(i) * GOLDEN_ANGLE;\n"
                  "    float r = factor * sqrt(float(i));\n"
                  "    vec2 pt = vec2(r * cos(theta), r * sin(theta));\n"
                  "    vec3 col = texture(texBackground, tcoords + pt).rgb;\n"
                  "    acc += col;\n"
                  "  }\n"
                  "  return acc / vec3(SAMPLES);\n"
                  "}\n"
                  "//VTK::FSQ::Decl";

        vtkShaderProgram::Substitute(FSSource, "//VTK::FSQ::Decl", ssDecl.str());

        std::stringstream ssImpl;
        ssImpl << "  vec4 mainSample = texture(texMain, texCoord);\n";
        if (UseBlurBackground) {
            ssImpl << "  vec3 bgCol = BokehBlur(texCoord, 0.05);\n";
        } else {
            ssImpl << "  vec3 bgCol = texture(texBackground, texCoord).rgb;\n";
        }
        ssImpl << "  vec3 result = bgCol * (1.0 - mainSample.a) + mainSample.rgb * mainSample.a;\n"
                  "  gl_FragData[0] = vec4(result.rgb, mainSample.a);\n";

        vtkShaderProgram::Substitute(FSSource, "//VTK::FSQ::Impl", ssImpl.str());

        // 创建四边形帮助器并初始化着色器
        BlendQuadHelper = std::make_shared<vtkOpenGLQuadHelper>(
          renWin, vtkOpenGLRenderUtilities::GetFullScreenQuadVertexShader().c_str(),
          FSSource.c_str(), "");
        BlendQuadHelper->ShaderChangeValue = GetMTime();
    } else {
        renWin->GetShaderCache()->ReadyShaderProgram(BlendQuadHelper->Program);
    }

    // 检查着色器编译是否成功
    if (!BlendQuadHelper->Program || !BlendQuadHelper->Program->GetCompiled()) {
        vtkErrorMacro("Couldn't build the blend shader program.");
        return;
    }

    // 设置背景纹理的边缘处理
    BackgroundPass->GetColorTexture()->SetWrapS(vtkTextureObject::ClampToEdge);
    BackgroundPass->GetColorTexture()->SetWrapT(vtkTextureObject::ClampToEdge);

    // 激活纹理并传递给着色器
    BackgroundPass->GetColorTexture()->Activate();
    MainPass->GetColorTexture()->Activate();
    BlendQuadHelper->Program->SetUniformi("texBackground", BackgroundPass->GetColorTexture()->GetTextureUnit());
    BlendQuadHelper->Program->SetUniformi("texMain", MainPass->GetColorTexture()->GetTextureUnit());

    // 执行渲染
    BlendQuadHelper->Render();

    // 停用纹理
    BackgroundPass->GetColorTexture()->Deactivate();
    MainPass->GetColorTexture()->Deactivate();
}
