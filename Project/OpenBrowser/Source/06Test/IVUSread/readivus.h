#ifndef READIVUS_H
#define READIVUS_H

#include "stable.h"
#include <vtkNew.h>
#include <vtkResliceImageViewer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkImagePlaneWidget.h>


#include <vtkImageMapper3D.h>
#include <vtkTransform.h>
#include <vtkImageProperty.h>
#include <vtkCornerAnnotation.h>
#include <vtkImageReslice.h>
#include <vtkImageProperty.h>



#include <vtkMatrix4x4.h>
#include <vtkTextProperty.h>
#include <vtkImageProperty.h>
#include <vtkCornerAnnotation.h>
#include <vtkImageReslice.h>
#include <vtkImageProperty.h>
#include <vtkImageSliceMapper.h>
#include <vtkImageActor.h>
#include <vtkCamera.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkImageCast.h>
#include <vtkCellPicker.h>
#include <vtkSmartPointer.h>
#include <vtkImageMapToColors.h>
#include <vtkResliceCursorWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkResliceCursorRepresentation.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkImageSeriesReader.h"
#include "itkGDCMSeriesFileNames.h"
#include <itkImageToVTKImageFilter.h>
#include <vtkSmartPointer.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkImageViewer2.h>
#include <vtkDICOMImageReader.h>
#include <vtkInteractorStyleImage.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>
#include <sstream>
#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkImageSeriesReader.h"
#include "itkGDCMSeriesFileNames.h"
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkAlgorithmOutput.h>
#include "vtkImplicitPlaneRepresentation.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkLookupTable.h"

namespace Ui {
    class ReadIVUS;
}


class ReadIVUS : public QWidget {
    Q_OBJECT

  public:
    explicit ReadIVUS(QWidget *parent = nullptr);
    ~ReadIVUS();
  private slots:
    void SlotLeftForward();
    void SlotLeftBackword();
    void SlotRightForward();
    void SlotRightBackword();
    void SlotEmpty() {}
    void on_pushButton_clicked();
    void SlotTransverseCurrentIndex(const int value);

    void on_spinbox_r_editingFinished();
    void on_pushButton_2_clicked();
    QImage VtkImageDataToQImage(vtkSmartPointer<vtkImageData> imageData);

  private:
    void LeftChangeAngle();

  private:
    Ui::ReadIVUS *ui;
    vtkNew< vtkImageViewer2> imageViewer ;
    vtkNew< vtkImagePlaneWidget > planeWidget_;
    vtkNew< vtkImageData > imagedata_;
    vtkNew< vtkRenderer > select_render_;
    vtkNew< vtkEventQtSlotConnect > vtk_connections_;
    vtkNew< vtkImageReslice > reslice ;
    vtkNew<vtkImageActor> imgActor ;
    qint32 angle;
};

#endif // READIVUS_H
