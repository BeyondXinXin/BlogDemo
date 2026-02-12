#ifndef VolumeCutter_H
#define VolumeCutter_H

#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderer.h>
#include <vtkPlanes.h>

#include <QImage>

class VolumeCutter
{
public:
    static void Cut(vtkImageData * volume, vtkRenderer * ren,
                    const QImage & mask, bool cutInside,
                    vtkPlanes * clipPlanes,
                    int airValue = -1024);
};

#endif