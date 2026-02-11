#include "imagesbrowermanager.h"


ImagesBrowerManager::ImagesBrowerManager(QObject *parent):
    QObject(parent) {
    this->Initial();
}

ImagesBrowerManager::~ImagesBrowerManager() {
    images_view1_->deleteLater();
    images_view2_->deleteLater();
}

QPointer<FormShow> ImagesBrowerManager::GetImagesView1() const {
    return images_view1_;
}

QPointer<FormShow> ImagesBrowerManager::GetImagesView2() const {
    return images_view2_;
}

void ImagesBrowerManager::SlotImgProcess(const int &operation, const QString &text) {
//    Q_UNUSED(text);
    QStringList filenames = QFileDialog::getOpenFileNames(
                                nullptr, tr("open image file"),
                                "./", tr("Image files(*.bmp *.jpg *.pbm "
                                         "*.pgm *.png *.ppm *.xbm *.xpm);;"
                                         "All files (*.*)"));
    if(1 == operation) {
        images_view1_->ShowImages(filenames);
    } else if(2 == operation) {
        images_view2_->ShowImages(filenames);
    }
}

void ImagesBrowerManager::Initial() {
    images_view1_ = new FormShow();
    images_view2_ = new FormShow();
    images_view1_->Initialization();
    images_view2_->Initialization();
}
