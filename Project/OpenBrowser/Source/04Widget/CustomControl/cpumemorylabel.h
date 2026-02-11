#ifndef CPUMEMORYLABEL_H
#define CPUMEMORYLABEL_H

/**
 * CPU内存显示控件
 * 1:实时显示当前CPU及内存使用情况,包括共多少内存已使用多少内存
 * 2:全平台通用,包括windows,linux,ARM
 */

// 01 Frame includes
#include "stable.h"

class QProcess;
class CpuMemoryLabel : public QLabel {
    Q_OBJECT
  public:
    CpuMemoryLabel(QWidget *parent = nullptr);
    ~CpuMemoryLabel();
    QSize sizeHint()                const;
    QSize minimumSizeHint()         const;

  public Q_SLOTS:
    void Start(qint32 interval);
    void Stop();

  Q_SIGNALS:
    void SIgnalTextChanged(const QString text);
    void SignalValueChanged(const qint32 cpu_percent_,
                            const qint32 memory_percent_,
                            const qint32 memory_all_,
                            const qint32 memory_use_,
                            const qint32 memory_free_);

  private slots:
    void SlotGetCPU();          //获取cpu
    void SlotGetMemory();       //获取内存
    void SlotReadData();        //读取数据
    void SlotSetData();         //设置数据

  private:
    qint32 total_new_, idle_new_, total_old_, idle_old_;
    qint32 cpu_percent_;
    qint32 memory_percent_;
    qint32 memory_all_;
    qint32 memory_use_;
    qint32 memory_free_;
    QTimer *timer_cpu_;       //定时器获取CPU信息
    QTimer *timer_memory_;    //定时器获取内存信息
    QProcess *process_;      //linux执行命令
};

#endif // CPUMEMORYLABEL_H
