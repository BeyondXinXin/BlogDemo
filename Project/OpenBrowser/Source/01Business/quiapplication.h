#ifndef QAUIAPPLICATION_H
#define QAUIAPPLICATION_H

/*
 * 仅能启动一个实例
*/

// 01 Frame includes
#include "stable.h"

class QUIApplication : public QApplication {
    Q_OBJECT

  public:
    QUIApplication(int &argc, char **argv);
    ~QUIApplication();
    bool lock();
  private:
    QSharedMemory *_singular; // shared memory !! SINGLE ACCESS
};

#endif // APP_H
