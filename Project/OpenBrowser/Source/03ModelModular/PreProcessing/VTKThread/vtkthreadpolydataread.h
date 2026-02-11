#ifndef VTKTHREADPOLYDATAREAD_H
#define VTKTHREADPOLYDATAREAD_H

/*
 * vtk线程  读取PolyData数据
*/

// 03 VTK includes
#include "vtkthread.h"

// VTK includes
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

class VtkThreadPolyDataRead  : public VtkThread {
    Q_OBJECT
  public:
    enum Format {
        UNKNOW,
        VTKXML,
        VTK,
        STL,
        PLY
    };

  public:
    explicit VtkThreadPolyDataRead(QObject *parent = nullptr);
    virtual ~VtkThreadPolyDataRead() override;
    void SetInputFileName(const QString &value);
    QString GetVolumeSurfaaceArea();
    vtkSmartPointer<vtkPolyData> GetOutput();

  protected:
    virtual void run() override;

  private:
    void Initial();
    bool ReadStlSurfaceFile();
    bool ReadVTPSurfaceFile();
    bool ReadVTKSurfaceFile();
    bool ReadPLYSurfaceFile();
    void Execute();

  private:
    Format format_;
    vtkSmartPointer<vtkPolyData> surface_;
    vtkSmartPointer<vtkPolyData> output_;
    QString input_file_name_;



};

#endif // VTKTHREADPOLYDATAREAD_H
