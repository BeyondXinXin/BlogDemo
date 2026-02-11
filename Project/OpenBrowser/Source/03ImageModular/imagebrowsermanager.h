#ifndef IMAGEBROWSERMANAGER_H
#define IMAGEBROWSERMANAGER_H

/*
 * image browser 模块 manager
*/

// 01 Frame includes
#include "stable.h"

// 03 OPENCV includes
#include "qopencvprocessing.h"
#include "qopencvwidget.h"

class ImageBrowserManager : public QObject
{
    Q_OBJECT
public:
    explicit ImageBrowserManager(QOpencvWidget &widget,
                                 QObject *parent = nullptr);
    virtual ~ImageBrowserManager() override;
    void OpenStlFile(const QString &file_path = "");
public Q_SLOTS:
    void SlotImgProcess(const int &operation, const QString &text);
Q_SIGNALS:
    void SignalPromptInformationOut(const QString &text);

private:
    void Initial();
    void UnDo();
    void ReDo();
    void UpDataImage(const QImage);

private:
    QOpencvWidget &widget_;
    QVector<QImage> img_vector_; // 存储图像的Vector容器
    QVector<QImage>::iterator imt_iter_; // Vector迭代器
};

#endif // IMAGEBROWSERMANAGER_H
