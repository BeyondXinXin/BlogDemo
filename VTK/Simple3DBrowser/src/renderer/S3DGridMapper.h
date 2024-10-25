#ifndef S3DGridMapper_h
#define S3DGridMapper_h

#include <vtkOpenGLPolyDataMapper.h>
#include <vtkSmartPointer.h>

class S3DGridMapper : public vtkOpenGLPolyDataMapper
{
public:
    static S3DGridMapper * New();
    vtkTypeMacro(S3DGridMapper, vtkOpenGLPolyDataMapper);

    vtkSetMacro(FadeDistance, double);
    vtkGetMacro(FadeDistance, double);

    vtkSetMacro(UnitSquare, double);
    vtkGetMacro(UnitSquare, double);

    double * GetBounds() override;

private:
    S3DGridMapper();
    ~S3DGridMapper() = default;
    S3DGridMapper(const S3DGridMapper &) = delete;
    void operator=(const S3DGridMapper &) = delete;

    void ReplaceShaderValues(std::map<vtkShader::Type, vtkShader *> shaders, vtkRenderer * ren, vtkActor * actor) override;
    void SetMapperShaderParameters(vtkOpenGLHelper & cellBO, vtkRenderer * ren, vtkActor * actor) override;
    void BuildBufferObjects(vtkRenderer * ren, vtkActor * act) override;
    void RenderPiece(vtkRenderer * ren, vtkActor * actor) override;
    bool GetNeedToRebuildShaders(vtkOpenGLHelper & cellBO, vtkRenderer * ren, vtkActor * act) override;

private:
    double FadeDistance = 10.0; // ½¥Òþ¾àÀë
    double UnitSquare = 1.0; // ±ê×¼»¯×ø±ê
};

#endif
