#ifndef VolumeCutterGPU_H
#define VolumeCutterGPU_H

#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkPlanes.h>

#include <QImage>
#include <QOpenGLFunctions>

class VolumeCutterGPU
{
public:
    static void Cut(vtkImageData * volume, vtkRenderer * ren,
                    vtkGenericOpenGLRenderWindow * renWin,
                    const QImage & mask, bool cutInside,
                    vtkPlanes * clipPlanes,
                    int airValue = -1024);

private:
    static GLuint CompileComputeShader();
    static bool s_shaderCompiled;
    static GLuint s_program;
};

#endif