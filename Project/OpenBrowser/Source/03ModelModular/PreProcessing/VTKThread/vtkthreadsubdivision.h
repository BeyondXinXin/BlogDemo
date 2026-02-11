#ifndef VTKTHREADSUBDIVISION_H
#define VTKTHREADSUBDIVISION_H

/*
 * vtk线程  polydata 表面细分
*/

// 03 VTK includes
#include "vtkthread.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

class VtkThreadSubdivision : public VtkThread {
    Q_OBJECT
  public:
    enum Method {
        LINEAR,
        BUTTERFLY,
        LOOP
    };
  public:
    explicit VtkThreadSubdivision(QObject *parent = nullptr);
    virtual ~VtkThreadSubdivision() override;

    void Execute();
    void SetSurface(const vtkSmartPointer<vtkPolyData> value);
    void SetNumberOfSubdivisions(const qint32 value);
    void SetMethod(const Method value);
    vtkSmartPointer<vtkPolyData> GetSurface() const;

  protected:
    virtual void run() override;
  private:
    void Initial();
  private:
    vtkSmartPointer<vtkPolyData> surface_;
    qint32 number_of_subdivisions_;
    Method method_;



};

#endif // VTKTHREADSUBDIVISION_H
