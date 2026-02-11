#ifndef OPENBROWSERAPPLICATION_H
#define OPENBROWSERAPPLICATION_H

/*
 * 控制整个程序的运行过程
 */

// 01 Frame includes
#include "stable.h"
#include "formtitle.h"
#include "quilogmanager.h"

class openBrowserApplication : public QObject {
    Q_OBJECT
  public:
    explicit openBrowserApplication(QObject *parent = nullptr);
    ~openBrowserApplication();
    bool Run();

  signals:

  public slots:
  private:
    void LogInitial(); // 日志界面初始化
    void openBrowserInitial(); // 主界面初始化
    void DirInitial(); // 程序路径初始化
    void DbInitial(); // 数据库初始化
    void FileInitial(); // 文件拷贝初始化
    void LicenseInitial(); // 注册文件初始化

  private:
    bool appReadyState_; // 初始化状态
    bool appRunState_; // 运行状态

    FormTitle *qui_;
    QUILogManager *log_manager_;

};

#endif // OPENBROWSERAPPLICATION_H
