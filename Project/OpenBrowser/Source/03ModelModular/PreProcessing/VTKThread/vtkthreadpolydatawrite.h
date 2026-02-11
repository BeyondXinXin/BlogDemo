#ifndef VTKTHREADPOLYDATAWRITE_H
#define VTKTHREADPOLYDATAWRITE_H

/*
 * vtk线程  写出PolyData数据(stl)
*/

// 03 VTK includes
#include "vtkthread.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

class VtkThreadPolyDataWrite : public VtkThread {
    Q_OBJECT
  public:
    enum Format {
        UNKNOW,
        VTKXML,
        VTK,
        STL,
        PLY,
        POINTDATA,
        TECPLOT
    };
    enum Mode {
        ASCII,
        BINARY
    };
  public:
    explicit VtkThreadPolyDataWrite(QObject *parent = nullptr);
    virtual ~VtkThreadPolyDataWrite() override;
    void Execute();
    void SetSurface(const vtkSmartPointer<vtkPolyData> value);
    void SetOutputFileName(const QString &value);
  protected:
    virtual void run() override;
  private:
    void Initial();
    bool WriteVtkSurfaceFile();
    bool WriteStlSurfaceFile();
  private:
    vtkSmartPointer<vtkPolyData> poly_data_;
    Format format_;
    Mode mode_;
    QString output_file_name_;





};

#endif // VTKTHREADPOLYDATAWRITE_H
