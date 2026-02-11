#ifndef IMAGESBROWERMANAGER_H
#define IMAGESBROWERMANAGER_H

// 01 Frame includes
#include "stable.h"
#include "formshow.h"

class ImagesBrowerManager : public QObject {
    Q_OBJECT
  public:
    explicit ImagesBrowerManager(QObject *parent = nullptr);
    virtual ~ImagesBrowerManager() override;
    QPointer<FormShow> GetImagesView1() const;
    QPointer<FormShow> GetImagesView2() const;
  public Q_SLOTS:
    void SlotImgProcess(const int &operation, const QString &text);
  private:
    void Initial();
  private:
    QPointer<FormShow> images_view1_;
    QPointer<FormShow> images_view2_;

};

#endif // IMAGESBROWERMANAGER_H
