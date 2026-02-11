#include "vtkEarthSphere.h"

#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkObjectFactory.h>
#include <vtkMath.h>

#include <cmath>
#include <iostream>

vtkStandardNewMacro(vtkEarthSphere);

vtkEarthSphere::vtkEarthSphere()
{
    m_TopographyTexture = vtkSmartPointer<vtkImageData>::New();
    m_BathymetryTexture = vtkSmartPointer<vtkImageData>::New();
}

void vtkEarthSphere::SetTopographyTexture(const std::string & imagePath)
{
    vtkNew<vtkImageReader2Factory> readerFactory;
    vtkSmartPointer<vtkImageReader2> imageReader;
    imageReader.TakeReference(readerFactory->CreateImageReader2(imagePath.c_str()));
    imageReader->SetFileName(imagePath.c_str());
    imageReader->Update();

    m_TopographyTexture->DeepCopy(imageReader->GetOutput());
}

void vtkEarthSphere::SetBathymetryTexture(const std::string & imagePath)
{
    vtkNew<vtkImageReader2Factory> readerFactory;
    vtkSmartPointer<vtkImageReader2> imageReader;
    imageReader.TakeReference(readerFactory->CreateImageReader2(imagePath.c_str()));
    imageReader->SetFileName(imagePath.c_str());
    imageReader->Update();

    m_BathymetryTexture->DeepCopy(imageReader->GetOutput());
}

void vtkEarthSphere::PrintSelf(std::ostream & os, vtkIndent indent)
{
}

double vtkEarthSphere::EvaluateFunction(double pos[3])
{
    if (m_TopographyTexture->GetNumberOfPoints() == 0 || m_BathymetryTexture->GetNumberOfPoints() == 0) {
        return -1000;
    }

    const int topoXDim = m_TopographyTexture->GetDimensions()[0] - 1;
    const int topoYDim = m_TopographyTexture->GetDimensions()[1] - 1;
    const int bathXDim = m_BathymetryTexture->GetDimensions()[0] - 1;
    const int bathYDim = m_BathymetryTexture->GetDimensions()[1] - 1;

    double tc[2] { .0 };
    double pixel = 0.0, phi = 0.0, thetaX = 0.0, thetaY = 0.0;

    const auto x = pos[0];
    const auto y = pos[1];
    const auto z = pos[2];
    const auto r = sqrt(x * x + y * y + z * z);

    if (abs(r - .8) < 0.02) {
        if (z > r) {
            phi = 0.0;
            if (z > 0.0) {
                tc[1] = 0.0;
            } else {
                tc[1] = 1.0;
            }
        } else {
            phi = acos(z / r);
            tc[1] = phi / vtkMath::Pi();
        }

        const auto rho = r * sin(phi);
        if (rho != 0.0) {
            if (abs(x) > rho) {
                if (x > 0.0) {
                    thetaX = 0.0;
                } else {
                    thetaX = vtkMath::Pi();
                }
            } else {
                thetaX = acos(x / rho);
            }
            if (abs(y) > rho) {
                if (y > 0.0) {
                    thetaY = vtkMath::Pi() / 2.0;
                } else {
                    thetaY = -vtkMath::Pi() / 2.0;
                }
            } else {
                thetaY = asin(y / rho);
            }
        } else {
            thetaX = thetaY = 0.0;
        }

        tc[0] = thetaX / (2.0 * vtkMath::Pi());
        if (thetaY < 0.0) {
            tc[0] = 1.0 - tc[0];
        }

        // 从地形图采样
        const auto topoTextureX = static_cast<int>(tc[0] * topoXDim);
        const auto topoTextureY = topoYDim - static_cast<int>(tc[1] * topoYDim);
        unsigned char * topoPixel = static_cast<unsigned char *>(m_TopographyTexture->GetScalarPointer(topoTextureX, topoTextureY, 0));

        const auto bathTextureX = static_cast<int>(tc[0] * bathXDim);
        const auto bathTextureY = bathYDim - static_cast<int>(tc[1] * bathYDim);
        unsigned char * bathPixel = static_cast<unsigned char *>(m_BathymetryTexture->GetScalarPointer(bathTextureX, bathTextureY, 0));
        double topoValue = 6400.0 * (static_cast<double>(*topoPixel) / 255.0);
        double bathValue = -8000.0 * (1.0 - static_cast<double>(*bathPixel) / 255.0);
        pixel = topoValue + bathValue;
    } else {
        pixel = 10000;
    }

    return pixel;
}

void vtkEarthSphere::EvaluateGradient(double x[3], double n[3])
{
}