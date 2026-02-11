#ifndef VTKTHREAD_H
#define VTKTHREAD_H

/*
 * vtk线程   操作基类
 * 仅有run函数在子线程,其余均在主线程
*/

// 01 Frame includes
#include "stable.h"

// VTK inludes
#include <vtkObject.h>

class VtkThread : public QThread {
    Q_OBJECT
  public:
    explicit VtkThread(QObject *parent = nullptr);
    virtual ~VtkThread() override;
    bool GetThreadResult() const;//获取结果
  signals:
    void SignalVtkThreadProgressOut(const QString str, const int value);

  protected:
    virtual void run() override;//线程
    void InitialResult();//初始化
    void SetResult(const bool result);//设置结果
    void PrintProgress(vtkObject *caller,
                       unsigned long vtk_event, void *call_data);
  protected:
    bool result_;//执行结果
};

#endif // VTKTHREAD_H
