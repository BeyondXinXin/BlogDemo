#ifndef GLOBVALUE_H
#define GLOBVALUE_H

/*
 * 全局变量控制
*/

#include "stable.h"

class QUIConfig : public QObject {
    Q_OBJECT
  public:
    struct CmdOption {
        bool debug_mode;
        bool test_mode;
        bool relese_mode;
    };
    static CmdOption cmd_option_;

  signals:

  public slots:
  private:
    explicit QUIConfig(QObject *parent = nullptr);
    virtual ~QUIConfig() override;
};

#endif // GLOBVALUE_H
