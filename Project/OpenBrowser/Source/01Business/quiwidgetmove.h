#ifndef QUIWIDGETMOVE_H
#define QUIWIDGETMOVE_H

/*
 * 界面支持拖拽
*/

// 01 Frame includes
#include "stable.h"

class QUIWidgetMove : public QObject {
    Q_OBJECT
  public:
    static QUIWidgetMove *Instance();
    explicit QUIWidgetMove(QObject *parent = nullptr);
    void Run();

  protected:
    bool eventFilter(QObject *obj, QEvent *evt);

  private:
    static QUIWidgetMove *self_;

  signals:

  public slots:
};

#endif // QUIWIDGETMOVE_H
