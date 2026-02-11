#ifndef ABOUTINFO_H
#define ABOUTINFO_H

/*
 * 界面左上角log，包含程序名称、版本、log
 */

// 01 Frame includes
#include "stable.h"


namespace Ui {
    class AboutInfo;
}

class AboutInfo : public QWidget {
    Q_OBJECT

  public:
    explicit AboutInfo(QWidget *parent = nullptr);
    virtual ~AboutInfo() override;
  private:
    void Initial();
  private:
    Ui::AboutInfo *ui;
};

#endif // ABOUTINFO_H
