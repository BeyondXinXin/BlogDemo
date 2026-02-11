#ifndef COMMANDLINEMANAGER_H
#define COMMANDLINEMANAGER_H

/*
 * 命令行管理manager
*/

// 01 Frame includes
#include "stable.h"

class CommandLineManager : public QObject {
    Q_OBJECT
  public:
    static void Initial(const QApplication &app);
  signals:

  public slots:

  private:
    explicit CommandLineManager(QObject *parent = nullptr);
    virtual ~CommandLineManager() override;
};

#endif // COMMANDLINEMANAGER_H
