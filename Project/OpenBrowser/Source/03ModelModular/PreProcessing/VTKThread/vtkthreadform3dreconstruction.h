#ifndef VTKTHREADFORM3DRECONSTRUCTION_H
#define VTKTHREADFORM3DRECONSTRUCTION_H

/*
 * vtk线程  重构三维数据
*/

// 03 VTK includes
#include "vtkthread.h"
//vtk
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkAlgorithmOutput.h>

class VtkThreadForm3DReconstruction : public VtkThread {
    Q_OBJECT
  public:
    explicit VtkThreadForm3DReconstruction(QObject *parent = nullptr);
    virtual ~VtkThreadForm3DReconstruction() override;
    void SetForm3DReconstructionValue(const QString &value);
    QString GetForm3DReconstructionValue();
  protected:
    virtual void run() override;
  private:
    void Initial();
    void Execute();
  private:
    QString reconstruction_value_;
};

#endif // VTKTHREADFORM3DRECONSTRUCTION_H
