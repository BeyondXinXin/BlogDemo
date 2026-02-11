#ifndef vtkEarthSphere_H
#define vtkEarthSphere_H

#include <vtkImplicitFunction.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>

class vtkEarthSphere : public vtkImplicitFunction
{
public:
    vtkTypeMacro(vtkEarthSphere, vtkImplicitFunction);
    void PrintSelf(ostream & os, vtkIndent indent) override;
    static vtkEarthSphere * New();
    using vtkImplicitFunction::EvaluateFunction;
    double EvaluateFunction(double pos[3]) override;
    void EvaluateGradient(double x[3], double n[3]) override;

    void SetTopographyTexture(const std::string & imagePath);
    void SetBathymetryTexture(const std::string & imagePath);

protected:
    vtkEarthSphere();
    ~vtkEarthSphere() override = default;

private:
    vtkEarthSphere(const vtkEarthSphere &) = delete;
    void operator=(const vtkEarthSphere &) = delete;

    vtkSmartPointer<vtkImageData> m_TopographyTexture;
    vtkSmartPointer<vtkImageData> m_BathymetryTexture;
};

#endif // vtkEarthSphere_H