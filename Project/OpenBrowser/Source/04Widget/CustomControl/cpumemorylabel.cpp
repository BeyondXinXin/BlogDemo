// 01 Frame includes
#include "cpumemorylabel.h"



#ifdef Q_OS_WIN
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x502
    #endif
    #include "windows.h"
#endif
#define MB (1024 * 1024)
#define KB (1024)

CpuMemoryLabel::CpuMemoryLabel(QWidget *parent) : QLabel(parent) {
    total_new_ = idle_new_ = total_old_ = idle_old_ = 0;
    cpu_percent_ = 0;
    memory_percent_ = 0;
    memory_all_ = 0;
    memory_use_ = 0;
    timer_cpu_ = new QTimer(this);
    connect(timer_cpu_, SIGNAL(timeout()), this, SLOT(SlotGetCPU()));
    timer_memory_ = new QTimer(this);
    connect(timer_memory_, SIGNAL(timeout()), this, SLOT(SlotGetMemory()));
    process_ = new QProcess(this);
    connect(process_, SIGNAL(readyRead()), this, SLOT(SlotReadData()));
}

CpuMemoryLabel::~CpuMemoryLabel() {
    Stop();
}

void CpuMemoryLabel::Start(qint32 interval) {
    SlotGetCPU();
    SlotGetMemory();
    timer_cpu_->start(interval);
    timer_memory_->start(interval + 200);
}

void CpuMemoryLabel::Stop() {
    timer_cpu_->stop();
    timer_memory_->stop();
}

void CpuMemoryLabel::SlotGetCPU() {
#ifdef Q_OS_WIN
    static FILETIME preidleTime;
    static FILETIME prekernelTime;
    static FILETIME preuserTime;
    FILETIME idleTime;
    FILETIME kernelTime;
    FILETIME userTime;
    GetSystemTimes(&idleTime, &kernelTime, &userTime);
    quint64 a, b;
    int idle, kernel, user;
    a = (preidleTime.dwHighDateTime << 31) | preidleTime.dwLowDateTime;
    b = (idleTime.dwHighDateTime << 31) | idleTime.dwLowDateTime;
    idle = b - a;
    a = (prekernelTime.dwHighDateTime << 31) | prekernelTime.dwLowDateTime;
    b = (kernelTime.dwHighDateTime << 31) | kernelTime.dwLowDateTime;
    kernel = b - a;
    a = (preuserTime.dwHighDateTime << 31) | preuserTime.dwLowDateTime;
    b = (userTime.dwHighDateTime << 31) | userTime.dwLowDateTime;
    user = b - a;
    cpu_percent_ = (kernel + user - idle) * 100 / (kernel + user);
    preidleTime = idleTime;
    prekernelTime = kernelTime;
    preuserTime = userTime ;
    SlotSetData();
#else
    if (process_->state() == QProcess::NotRunning) {
        total_new_ = idle_new_ = 0;
        process_->start("cat /proc/stat");
    }
#endif
}

void CpuMemoryLabel::SlotGetMemory() {
#ifdef Q_OS_WIN
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    memory_percent_ = statex.dwMemoryLoad;
    memory_all_ = statex.ullTotalPhys / MB;
    memory_free_ = statex.ullAvailPhys / MB;
    memory_use_ = memory_all_ - memory_free_;
    SlotSetData();
#else
    if (process_->state() == QProcess::NotRunning) {
        process_->start("cat /proc/meminfo");
    }
#endif
}

void CpuMemoryLabel::SlotReadData() {
    while (!process_->atEnd()) {
        QString s = QLatin1String(process_->readLine());
        if (s.startsWith("cpu")) {
            QStringList list = s.split(" ");
            idle_new_ = list.at(5).toInt();
            foreach (QString value, list) {
                total_new_ += value.toInt();
            }
            int total = total_new_ - total_old_;
            int idle = idle_new_ - idle_old_;
            cpu_percent_ = 100 * (total - idle) / total;
            total_old_ = total_new_;
            idle_old_ = idle_new_;
            break;
        } else if (s.startsWith("MemTotal")) {
            s = s.replace(" ", "");
            s = s.split(":").at(1);
            memory_all_ = s.left(s.length() - 3).toInt() / KB;
        } else if (s.startsWith("MemFree")) {
            s = s.replace(" ", "");
            s = s.split(":").at(1);
            memory_free_ = s.left(s.length() - 3).toInt() / KB;
        } else if (s.startsWith("Buffers")) {
            s = s.replace(" ", "");
            s = s.split(":").at(1);
            memory_free_ += s.left(s.length() - 3).toInt() / KB;
        } else if (s.startsWith("Cached")) {
            s = s.replace(" ", "");
            s = s.split(":").at(1);
            memory_free_ += s.left(s.length() - 3).toInt() / KB;
            memory_use_ = memory_all_ - memory_free_;
            memory_percent_ = 100 * memory_use_ / memory_all_;
            break;
        }
    }
    SlotSetData();
}

void CpuMemoryLabel::SlotSetData() {
    cpu_percent_ = (cpu_percent_ < 0 ? 0 : cpu_percent_);
    QString msg = QString("CPU %1%  Mem %2%")
                  .arg(cpu_percent_).arg(memory_percent_);
    this->setText(msg);
    emit SIgnalTextChanged(msg);
    emit SignalValueChanged(cpu_percent_, memory_percent_,
                            memory_all_, memory_use_, memory_free_);
}

QSize CpuMemoryLabel::sizeHint() const {
    return QSize(300, 30);
}

QSize CpuMemoryLabel::minimumSizeHint() const {
    return QSize(30, 10);
}
