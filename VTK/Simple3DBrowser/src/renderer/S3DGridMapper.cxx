#include "S3DGridMapper.h"

#include <vtkFloatArray.h>
#include <vtkInformation.h>
#include <vtkObjectFactory.h>
#include <vtkOpenGLError.h>
#include <vtkOpenGLRenderPass.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkOpenGLVertexArrayObject.h>
#include <vtkOpenGLVertexBufferObjectGroup.h>
#include <vtkRenderer.h>
#include <vtkShaderProgram.h>

vtkStandardNewMacro(S3DGridMapper);

S3DGridMapper::S3DGridMapper()
{
    SetNumberOfInputPorts(0);
    StaticOn();
}

double * S3DGridMapper::GetBounds()
{
    for (int i = 0; i < 3; ++i) {
        Bounds[2 * i] = -FadeDistance, Bounds[2 * i + 1] = FadeDistance;
    }
    return Bounds;
}

/**
 * @brief 替换着色器的源代码，以修改渲染效果。
 *
 * 根据当前场景（渲染器和 actor）的需求，通过替换顶点着色器和片段着色器的代码，动态调整渲染逻辑。
 * @param shaders 包含顶点和片段着色器的映射表。
 * @param ren 当前使用的渲染器。
 * @param actor 正在渲染的3D对象。
 */
void S3DGridMapper::ReplaceShaderValues(
  std::map<vtkShader::Type, vtkShader *> shaders, vtkRenderer * ren, vtkActor * actor)
{
    ReplaceShaderRenderPass(shaders, ren, actor, true);

    std::string VSSource = shaders[vtkShader::Vertex]->GetSource();
    std::string FSSource = shaders[vtkShader::Fragment]->GetSource();

    // 顶点着色器替换
    vtkShaderProgram::Substitute(
      VSSource,
      "//VTK::PositionVC::Dec",
      "out vec4 positionMCVSOutput;\n");

    vtkShaderProgram::Substitute(
      VSSource,
      "//VTK::PositionVC::Impl",
      "positionMCVSOutput = vec4(vertexMC.x, vertexMC.y, 0.0, 1.0);\n"
      "gl_Position = MCDCMatrix * positionMCVSOutput;\n");

    // 片段着色器替换
    vtkShaderProgram::Substitute(
      FSSource,
      "//VTK::PositionVC::Dec",
      "in vec4 positionMCVSOutput;\n"
      "uniform float fadeDist;\n"
      "uniform float unitSquare;\n");

    vtkShaderProgram::Substitute(
      FSSource,
      "//VTK::UniformFlow::Impl",
      "vec2 coord = positionMCVSOutput.xy / (unitSquare * positionMCVSOutput.w);\n"
      "vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);\n");

    vtkShaderProgram::Substitute(
      FSSource,
      "//VTK::Color::Impl",
      "float line = min(grid.x, grid.y);\n"
      "float dist2 = unitSquare * unitSquare * dot(coord, coord);\n"
      "float opacity = (1.0 - min(line, 1.0)) * (1.0 - dist2 / (fadeDist * fadeDist));\n"
      "vec3 color = diffuseColorUniform;\n"
      "if (abs(coord.x) < 0.1 && grid.y != line) color = vec3(0.0, 0.0, 1.0);\n"
      "if (abs(coord.y) < 0.1 && grid.x != line) color = vec3(1.0, 0.0, 0.0);\n"
      "gl_FragData[0] = vec4(color, opacity);\n");

    shaders[vtkShader::Vertex]->SetSource(VSSource);
    shaders[vtkShader::Fragment]->SetSource(FSSource);

    ReplaceShaderPositionVC(shaders, ren, actor);
    ReplaceShaderColor(shaders, ren, actor);
    ReplaceShaderRenderPass(shaders, ren, actor, false);
}

/**
 * @brief 设置渲染时所需的着色器参数。
 *
 * 为当前的 OpenGL 着色器程序绑定必要的参数（如 VAO），并将传递的 uniform 参数用于自定义渲染效果。
 * @param cellBO OpenGL 渲染助手，包含着色器和 VAO 等对象。
 * @param ren 当前的渲染器对象。
 * @param actor 正在渲染的3D对象。
 */
void S3DGridMapper::SetMapperShaderParameters(
  vtkOpenGLHelper & cellBO, vtkRenderer *, vtkActor * actor)
{

    if (VBOs->GetMTime() > cellBO.AttributeUpdateTime || cellBO.ShaderSourceTime > cellBO.AttributeUpdateTime) {
        cellBO.VAO->Bind();
        VBOs->AddAllAttributesToVAO(cellBO.Program, cellBO.VAO);
        cellBO.AttributeUpdateTime.Modified();
    }

    if (auto * info = actor->GetPropertyKeys()) {
        for (int i = 0, n = info->Length(vtkOpenGLRenderPass::RenderPasses()); i < n; ++i) {
            if (auto * rp = vtkOpenGLRenderPass::SafeDownCast(
                  info->Get(vtkOpenGLRenderPass::RenderPasses(), i))) {
                if (!rp->SetShaderParameters(cellBO.Program, this, actor, cellBO.VAO)) {
                    vtkErrorMacro("RenderPass::SetShaderParameters failed for " << rp->GetClassName());
                }
            }
        }
    }

    cellBO.Program->SetUniformf("fadeDist", FadeDistance);
    cellBO.Program->SetUniformf("unitSquare", UnitSquare);
}

/**
 * @brief 构建用于渲染的缓冲对象（VBO）。
 *
 * 在 GPU 上创建并填充顶点缓冲对象（VBO），存储平面顶点数据，供后续的绘制使用。
 * @param ren 当前的渲染器对象。
 * @param act 正在渲染的3D对象（此处未使用）。
 */
void S3DGridMapper::BuildBufferObjects(vtkRenderer * ren, vtkActor *)
{
    vtkNew<vtkFloatArray> infinitePlane;
    infinitePlane->SetNumberOfComponents(2);
    infinitePlane->SetNumberOfTuples(4);

    float d = static_cast<float>(FadeDistance);
    float corners[4][2] = { { -d, -d }, { d, -d }, { -d, d }, { d, d } };
    for (int i = 0; i < 4; ++i) {
        infinitePlane->SetTuple(i, corners[i]);
    }

    auto * cache = vtkOpenGLRenderWindow::SafeDownCast(ren->GetRenderWindow())->GetVBOCache();
    VBOs->CacheDataArray("vertexMC", infinitePlane, cache, VTK_FLOAT);
    VBOs->BuildAllVBOs(cache);

    vtkOpenGLCheckErrorMacro("Failed after BuildBufferObjects");
    VBOBuildTime.Modified();
}

/**
 * @brief 渲染当前对象的一个部分（或全部）。
 *
 * 调用 OpenGL 的绘制命令来渲染 3D 对象的几何体。此方法确保缓冲对象和着色器已更新，并使用 `glDrawArrays` 进行绘制。
 * @param ren 当前的渲染器对象。
 * @param actor 正在渲染的3D对象。
 */
void S3DGridMapper::RenderPiece(vtkRenderer * ren, vtkActor * actor)
{
    UpdateBufferObjects(ren, actor);
    UpdateShaders(Primitives[PrimitivePoints], ren, actor);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

/**
 * @brief 判断是否需要重建着色器程序。
 *
 * 检查着色器的修改时间与渲染过程的阶段时间，决定是否需要重编译着色器，以保证渲染效果的一致性。
 * @param cellBO OpenGL 渲染助手，包含当前的着色器程序和状态。
 * @param ren 当前的渲染器对象（未使用）。
 * @param act 正在渲染的3D对象。
 * @return 如果需要重建着色器，返回 true，否则返回 false。
 */
bool S3DGridMapper::GetNeedToRebuildShaders(
  vtkOpenGLHelper & cellBO, vtkRenderer *, vtkActor * actor)
{
    return !cellBO.Program || cellBO.ShaderSourceTime < GetRenderPassStageMTime(actor, &cellBO);
}
