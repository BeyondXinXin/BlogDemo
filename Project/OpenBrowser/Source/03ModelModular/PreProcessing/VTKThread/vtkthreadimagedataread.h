#ifndef VTKTHREADIMAGEDATAREAD_H
#define VTKTHREADIMAGEDATAREAD_H

/*
 * vtk线程  读取ImageData数据
*/

// 03 VTK includes
#include "vtkthread.h"

//vtk
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkAlgorithmOutput.h>


class VtkThreadImageDataRead : public VtkThread {
    Q_OBJECT
  public:
    enum Format {
        UNKNOW,
        VTKXML,
        VTK,
        DICOM,
        RAW,
        META,
        TIFF,
        PNG
    };//图像格式
  public:
    explicit VtkThreadImageDataRead(QObject *parent = nullptr);
    virtual ~VtkThreadImageDataRead() override;
    void SetInputFileName(const QString &value);
    vtkSmartPointer<vtkImageData> GetImageData() ;
  protected:
    virtual void run() override;
  private:
    void Initial();
    bool ReadDicom();
    void Execute();
  private:
    Format format_;
    vtkNew<vtkImageData> imagedata_;
    QString input_filename_;
    QList<double> data_spacing_;
    QList<double> data_origin_;
};

#endif // VTKTHREADIMAGEDATAREAD_H
