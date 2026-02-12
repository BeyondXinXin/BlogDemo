#include "VolumeCutter.h"

#include <vtkCamera.h>
#include <vtkMatrix4x4.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>

#include <QtConcurrent>
#include <QImage>
#include <QElapsedTimer>

void VolumeCutter::Cut(vtkImageData * volume, vtkRenderer * ren,
                       const QImage & mask, bool cutInside,
                       vtkPlanes * clipPlanes, int airValue)
{
    if (!volume || !ren) {
        return;
    }

    QElapsedTimer timer;
    timer.start();

    int dims[3];
    double origin[3], spacing[3];
    volume->GetDimensions(dims);
    volume->GetOrigin(origin);
    volume->GetSpacing(spacing);

    vtkCamera * cam = ren->GetActiveCamera();

    vtkNew<vtkMatrix4x4> view;
    view->DeepCopy(cam->GetViewTransformMatrix());

    double clippingRange[2];
    cam->GetClippingRange(clippingRange);

    vtkNew<vtkMatrix4x4> proj;
    proj->DeepCopy(cam->GetProjectionTransformMatrix(ren->GetTiledAspectRatio(), clippingRange[0], clippingRange[1]));

    vtkNew<vtkMatrix4x4> mvp;
    vtkMatrix4x4::Multiply4x4(proj, view, mvp);

    QImage grayMask = mask.convertToFormat(QImage::Format_Grayscale8);
    const int maskW = grayMask.width();
    const int maskH = grayMask.height();
    const uchar * maskBits = grayMask.constBits();
    const int maskBpl = grayMask.bytesPerLine();

    int * scalars = static_cast<int *>(volume->GetPointData()->GetScalars()->GetVoidPointer(0));

    // 按 Z 切片并行处理
    QVector<int> slices(dims[2]);
    std::iota(slices.begin(), slices.end(), 0);

    QtConcurrent::blockingMap(slices, [&](int z) {
        double wz = origin[2] + z * spacing[2];

        for (int y = 0; y < dims[1]; ++y) {
            double wy = origin[1] + y * spacing[1];

            for (int x = 0; x < dims[0]; ++x) {
                double wx = origin[0] + x * spacing[0];

                double pt[3] = { wx, wy, wz };
                if (clipPlanes->EvaluateFunction(pt) > 0.0) {
                    continue;
                }

                double clipX = mvp->GetElement(0, 0) * wx + mvp->GetElement(0, 1) * wy
                  + mvp->GetElement(0, 2) * wz + mvp->GetElement(0, 3);
                double clipY = mvp->GetElement(1, 0) * wx + mvp->GetElement(1, 1) * wy
                  + mvp->GetElement(1, 2) * wz + mvp->GetElement(1, 3);
                double clipW = mvp->GetElement(3, 0) * wx + mvp->GetElement(3, 1) * wy
                  + mvp->GetElement(3, 2) * wz + mvp->GetElement(3, 3);

                if (qFuzzyIsNull(clipW)) {
                    continue;
                }

                double ndcX = clipX / clipW;
                double ndcY = clipY / clipW;

                // NDC -> 屏幕 UV [0,1]
                double u = ndcX * 0.5 + 0.5;
                double v = ndcY * 0.5 + 0.5;

                if (u < 0.0 || u > 1.0 || v < 0.0 || v > 1.0) {
                    if (!cutInside) {
                        vtkIdType idx = z * (vtkIdType)dims[1] * dims[0] + y * (vtkIdType)dims[0] + x;
                        scalars[idx] = airValue;
                    }
                    continue;
                }

                int px = qBound(0, (int)(u * (maskW - 1)), maskW - 1);
                int py = qBound(0, (int)((1.0 - v) * (maskH - 1)), maskH - 1); // Y翻转

                uchar maskVal = maskBits[py * maskBpl + px];
                bool inMask = (maskVal > 127);

                bool shouldCut = cutInside ? inMask : !inMask;

                if (shouldCut) {
                    vtkIdType idx = z * (vtkIdType)dims[1] * dims[0]
                      + y * (vtkIdType)dims[0] + x;
                    scalars[idx] = airValue;
                }
            }
        }
    });

    volume->Modified();

    qDebug() << "VolumeCutter " << timer.elapsed() << "ms";
}