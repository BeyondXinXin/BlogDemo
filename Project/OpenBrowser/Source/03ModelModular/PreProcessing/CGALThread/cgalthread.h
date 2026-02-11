#ifndef CGALTHREAD_H
#define CGALTHREAD_H

/*
 * CGAL线程   操作基类
 * 仅有run函数在子线程,其余均在主线程
*/

// 01 Frame includes
#include "stable.h"

class CGALThread : public QThread {
    Q_OBJECT
  public:
    explicit CGALThread(QObject *parent = nullptr);
    virtual ~CGALThread() override;
    bool GetThreadResult() const;//获取结果
  protected:
    virtual void run() override;//线程
    void InitialResult();//初始化
    void SetResult(const bool result);//设置结果
  protected:
    bool result_;// 执行结果
};

#endif // CGALTHREAD_H
