#include "VolumeCutterGPU.h"

#include <vtkCamera.h>
#include <vtkMatrix4x4.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkNew.h>
#include <vtkPoints.h>

#include <QFile>
#include <QElapsedTimer>
#include <QOpenGLContext>
#include <QOpenGLVersionFunctionsFactory>
#include <QOpenGLFunctions_4_3_Core>

#include <vector>

bool VolumeCutterGPU::s_shaderCompiled = false;
GLuint VolumeCutterGPU::s_program = 0;

static QOpenGLFunctions_4_3_Core * getGL43()
{
    return QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_3_Core>(
      QOpenGLContext::currentContext());
}

GLuint VolumeCutterGPU::CompileComputeShader()
{
    QFile f(":/shader/volume_cut.comp");
    if (!f.open(QIODevice::ReadOnly)) {
        return 0;
    }

    QByteArray src = f.readAll();
    if (src.isEmpty()) {
        return 0;
    }

    auto * gl = getGL43();
    if (!gl) {
        return 0;
    }

    GLuint shader = gl->glCreateShader(GL_COMPUTE_SHADER);
    const char * srcPtr = src.constData();
    gl->glShaderSource(shader, 1, &srcPtr, nullptr);
    gl->glCompileShader(shader);

    GLint ok = 0;
    gl->glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        gl->glDeleteShader(shader);
        return 0;
    }

    GLuint prog = gl->glCreateProgram();
    gl->glAttachShader(prog, shader);
    gl->glLinkProgram(prog);
    gl->glDeleteShader(shader);

    gl->glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        gl->glDeleteProgram(prog);
        return 0;
    }

    return prog;
}

void VolumeCutterGPU::Cut(vtkImageData * volume, vtkRenderer * ren,
                          vtkGenericOpenGLRenderWindow * renWin,
                          const QImage & mask, bool cutInside,
                          vtkPlanes * clipPlanes, int airValue)
{
    QElapsedTimer timer;
    timer.start();

    if (!volume || !ren || !renWin || mask.isNull()) {
        return;
    }

    renWin->MakeCurrent();

    auto * gl = getGL43();
    if (!gl) {
        return;
    }

    // 编译 shader
    if (!s_shaderCompiled) {
        s_program = CompileComputeShader();
        s_shaderCompiled = (s_program != 0);
        if (!s_shaderCompiled) {
            return;
        }
    }

    int dims[3];
    double origin[3], spacing[3];
    volume->GetDimensions(dims);
    volume->GetOrigin(origin);
    volume->GetSpacing(spacing);

    int * scalars = static_cast<int *>(volume->GetPointData()->GetScalars()->GetVoidPointer(0));

    const vtkIdType totalVoxels = static_cast<vtkIdType>(dims[0]) * dims[1] * dims[2];

    // MVP
    vtkCamera * cam = ren->GetActiveCamera();
    vtkNew<vtkMatrix4x4> view;
    view->DeepCopy(cam->GetViewTransformMatrix());

    double clippingRange[2];
    cam->GetClippingRange(clippingRange);

    vtkNew<vtkMatrix4x4> proj;
    proj->DeepCopy(cam->GetProjectionTransformMatrix(
      ren->GetTiledAspectRatio(), clippingRange[0], clippingRange[1]));

    vtkNew<vtkMatrix4x4> mvp;
    vtkMatrix4x4::Multiply4x4(proj, view, mvp);

    float glMVP[16];
    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) {
            glMVP[c * 4 + r] = static_cast<float>(mvp->GetElement(r, c));
        }
    }

    // 裁剪平面
    float planeData[24] = { 0 }; // 6个平面 × 4个分量
    if (clipPlanes && clipPlanes->GetNumberOfPlanes() == 6) {
        vtkPoints * points = clipPlanes->GetPoints();
        vtkDataArray * normals = clipPlanes->GetNormals();

        for (int i = 0; i < 6; ++i) {
            double normal[3];
            normals->GetTuple(i, normal);

            double origin[3];
            points->GetPoint(i, origin);

            // 平面方程: n·(p - p0) = 0 => n·p + d = 0, d = -n·p0
            planeData[i * 4 + 0] = static_cast<float>(normal[0]);
            planeData[i * 4 + 1] = static_cast<float>(normal[1]);
            planeData[i * 4 + 2] = static_cast<float>(normal[2]);
            planeData[i * 4 + 3] = static_cast<float>(-(normal[0] * origin[0] + normal[1] * origin[1] + normal[2] * origin[2]));
        }
    }

    // SSBO
    GLuint ssbo = 0;
    gl->glGenBuffers(1, &ssbo);
    gl->glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    gl->glBufferData(GL_SHADER_STORAGE_BUFFER, totalVoxels * sizeof(int), nullptr, GL_DYNAMIC_COPY); // 不立即上传数据

    // 使用 map 上传数据，避免额外拷贝
    int * uploadPtr = static_cast<int *>(
      gl->glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, totalVoxels * sizeof(int), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

    if (uploadPtr) {
        std::memcpy(uploadPtr, scalars, totalVoxels * sizeof(int));
        gl->glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    } else {
        gl->glDeleteBuffers(1, &ssbo);
        return;
    }

    gl->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    // Mask
    QImage grayMask = mask.convertToFormat(QImage::Format_Grayscale8);

    GLint oldUnpack = 0, oldRowLength = 0;
    gl->glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldUnpack);
    gl->glGetIntegerv(GL_UNPACK_ROW_LENGTH, &oldRowLength);
    gl->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    gl->glPixelStorei(GL_UNPACK_ROW_LENGTH, grayMask.bytesPerLine());

    GLuint maskTex = 0;
    gl->glGenTextures(1, &maskTex);
    gl->glActiveTexture(GL_TEXTURE0);
    gl->glBindTexture(GL_TEXTURE_2D, maskTex);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, grayMask.width(), grayMask.height(), 0, GL_RED, GL_UNSIGNED_BYTE, grayMask.constBits());

    gl->glPixelStorei(GL_UNPACK_ROW_LENGTH, oldRowLength);
    gl->glPixelStorei(GL_UNPACK_ALIGNMENT, oldUnpack);

    // 设置 uniform
    gl->glUseProgram(s_program);
    gl->glUniformMatrix4fv(gl->glGetUniformLocation(s_program, "uMVP"), 1, GL_FALSE, glMVP);
    gl->glUniform3i(gl->glGetUniformLocation(s_program, "uDims"), dims[0], dims[1], dims[2]);
    gl->glUniform3f(gl->glGetUniformLocation(s_program, "uOrigin"), static_cast<float>(origin[0]), static_cast<float>(origin[1]), static_cast<float>(origin[2]));
    gl->glUniform3f(gl->glGetUniformLocation(s_program, "uSpacing"), static_cast<float>(spacing[0]), static_cast<float>(spacing[1]), static_cast<float>(spacing[2]));
    gl->glUniform1i(gl->glGetUniformLocation(s_program, "uCutInside"), cutInside ? 1 : 0);
    gl->glUniform1i(gl->glGetUniformLocation(s_program, "uAirValue"), static_cast<int>(airValue));
    gl->glUniform1i(gl->glGetUniformLocation(s_program, "uMask"), 0);
    gl->glUniform4fv(gl->glGetUniformLocation(s_program, "uClipPlanes"), 6, planeData);

    // 执行 compute
    gl->glDispatchCompute((dims[0] + 15) / 16, (dims[1] + 7) / 8, (dims[2] + 3) / 4);
    gl->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // 结果回读
    gl->glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

    int * downloadPtr = static_cast<int *>(
      gl->glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, totalVoxels * sizeof(int), GL_MAP_READ_BIT));

    if (downloadPtr) {
        std::memcpy(scalars, downloadPtr, totalVoxels * sizeof(int));
        gl->glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    } else {
        qWarning() << "Failed to map SSBO for download";
    }

    // 结束
    gl->glDeleteBuffers(1, &ssbo);
    gl->glDeleteTextures(1, &maskTex);

    volume->Modified();

    qDebug() << "VolumeCutterGPU " << timer.elapsed() << "ms";
}