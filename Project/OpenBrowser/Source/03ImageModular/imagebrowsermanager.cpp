// 01 Frame includes
#include "imagebrowsermanager.h"
#include "quihelper.h"

// 06 Test includes
#include "readivus.h"
//#include "mainwindow.h"
#include "formedgeadjustment.h"

ImageBrowserManager::ImageBrowserManager(QOpencvWidget &widget,
                                         QObject *parent)
  : QObject(parent)
  , widget_(widget)
{
    this->Initial();
}

ImageBrowserManager::~ImageBrowserManager()
{
}

void ImageBrowserManager::Initial()
{
}

void ImageBrowserManager::UnDo()
{ // 撤销
    if (imt_iter_ != img_vector_.begin()) { // 前面还有对象
        imt_iter_--;
        this->widget_.SetSurface(*imt_iter_);
        if (imt_iter_ == img_vector_.begin()) { // 自减为初始图像
            if (imt_iter_ != (img_vector_.end() - 1)) {
                img_vector_.erase(++imt_iter_, img_vector_.end());
            }
        }
    }
}

void ImageBrowserManager::ReDo()
{ // 重做
    if (imt_iter_ != img_vector_.end() && imt_iter_ != img_vector_.end() - 1) {
        // 后面还有对象
        imt_iter_++;
        this->widget_.SetSurface(*imt_iter_);
        if (imt_iter_ == (img_vector_.end())) { // 自加后是否为最后一个
        }
    }
}

void ImageBrowserManager::UpDataImage(const QImage tmp)
{
    img_vector_.append(tmp);
    imt_iter_ = img_vector_.end() - 1;
    this->widget_.SetSurface(*imt_iter_);
}

void ImageBrowserManager::OpenStlFile(const QString &file_path)
{
    QString tmp = file_path;
    QImage *img = new QImage();
    if (tmp.isEmpty()) {
        tmp = QUIHelper::GetFileName(
          "*.bmp *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm ");
    }
    if (!(img->load(tmp))) {
        QUIHelper::ShowMessageBoxError("打开图像失败！", 3, true);
        delete img;
        return;
    }
    this->widget_.SetSurface(*img);
    img_vector_.clear();
    img_vector_.append(*img);
    imt_iter_ = img_vector_.end() - 1;
    this->widget_.SetSurface(*imt_iter_);
}

void ImageBrowserManager::SlotImgProcess(const int &operation, const QString &text)
{
    if (this->img_vector_.size() == 0) {
        emit SignalPromptInformationOut(("Image 数据错误"));
        return;
    }
    QString tmp_information = text;
    QStringList imformation = tmp_information.split("|");
    switch (operation) {
    case 1: { // 载入
        OpenStlFile();
        break;
    }
    case 2: { // 保存
        QImage img = QOpencvProcessing::Instance()->splitColor(
          widget_.GetSurface(),
          imformation.at(0).toLocal8Bit().data(),
          imformation.at(1).toInt());
        UpDataImage(img);
        break;
    }
    case 3: { // 撤销
        UnDo();
        break;
    }
    case 4: { // 重做
        ReDo();
        break;
    }
    case 101: { // 图像自适应大小
        emit SignalPromptInformationOut(("图像自适应大小"));
        break;
    }
    case 102: { // 改变大小
        QImage img = widget_.GetSurface();
        QOpencvProcessing::Instance()->Resize(
          img, imformation.at(0).toInt(), imformation.at(1).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("改变大小"));
        break;
    }
    case 103: { // 确认缩放
        QImage img = widget_.GetSurface();
        QOpencvProcessing::Instance()->Enlarge_Reduce(
          img, imformation.at(0).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("确认缩放"));
        break;
    }
    case 104: { // 确认旋转
        QImage img = widget_.GetSurface();
        QOpencvProcessing::Instance()->Rotate(img, imformation.at(0).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("确认旋转"));
        break;
    }
    case 105: { // 确认线性变换
        QImage img = widget_.GetSurface();
        QOpencvProcessing::Instance()->Lean(
          img, imformation.at(0).toInt(), imformation.at(1).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("确认线性变换"));
        break;
    }
    case 106: { // 水平镜像
        QImage img = widget_.GetSurface();
        QOpencvProcessing::Instance()->Flip(img, 1);
        UpDataImage(img);
        emit SignalPromptInformationOut(("水平镜像"));
        break;
    }
    case 107: { // 垂直镜像
        QImage img = widget_.GetSurface();
        QOpencvProcessing::Instance()->Flip(img, 0);
        UpDataImage(img);
        emit SignalPromptInformationOut(("垂直镜像"));
        break;
    }
    case 201: { // 二值图像
        QImage img = QOpencvProcessing::Instance()->Bin(
          widget_.GetSurface(),
          imformation.at(0).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("二值图像"));
        break;
    }
    case 202: { // 灰度图像
        QImage img = QOpencvProcessing::Instance()->Graylevel(
          widget_.GetSurface());
        UpDataImage(img);
        emit SignalPromptInformationOut(("灰度图像"));
        break;
    }
    case 203: { // 图像反转
        QImage img = QOpencvProcessing::Instance()->Reverse(
          widget_.GetSurface());
        UpDataImage(img);
        emit SignalPromptInformationOut(("图像反转"));
        break;
    }
    case 204: { // 确认线性变换
        QImage img = QOpencvProcessing::Instance()->Linear(
          widget_.GetSurface(),
          imformation.at(0).toInt(),
          imformation.at(1).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("确认线性变换"));
        break;
    }
    case 205: { // 确认对数变换
        QImage img = QOpencvProcessing::Instance()->Log(
          widget_.GetSurface(),
          imformation.at(0).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("确认对数变换"));
        break;
    }
    case 206: { // 确认伽马变换
        QImage img = QOpencvProcessing::Instance()->Gamma(
          widget_.GetSurface(),
          imformation.at(0).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("确认伽马变换"));
        break;
    }
    case 207: { // 直方图均衡化
        QImage img = QOpencvProcessing::Instance()->Histeq(
          widget_.GetSurface());
        UpDataImage(img);
        emit SignalPromptInformationOut(("直方图均衡化"));
        break;
    }
    case 301: { // 简单滤波
        QImage img = QOpencvProcessing::Instance()->Normalized(
          widget_.GetSurface(),
          imformation.at(0).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("简单滤波"));
        break;
    }
    case 302: { // 高斯滤波
        QImage img = QOpencvProcessing::Instance()->Gaussian(
          widget_.GetSurface(),
          imformation.at(0).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("高斯滤波"));
        break;
    }
    case 303: { // 中值滤波
        QImage img = QOpencvProcessing::Instance()->Median(
          widget_.GetSurface(),
          imformation.at(0).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("中值滤波"));
        break;
    }
    case 304: { // sobel边缘检测
        QImage img = QOpencvProcessing::Instance()->Sobel(
          widget_.GetSurface(),
          imformation.at(0).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("sobel边缘检测"));
        break;
    }
    case 305: { // laplacian边缘检测
        QImage img = QOpencvProcessing::Instance()->Laplacian(
          widget_.GetSurface(),
          imformation.at(0).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("laplacian边缘检测"));
        break;
    }
    case 306: { // canny边缘检测
        QImage img = QOpencvProcessing::Instance()->Canny(
          widget_.GetSurface(),
          imformation.at(0).toInt(),
          imformation.at(1).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("canny边缘检测"));
        break;
    }
    case 307: { // line检测
        QImage img = QOpencvProcessing::Instance()->HoughLine(
          widget_.GetSurface(),
          imformation.at(0).toInt(),
          imformation.at(1).toInt(),
          imformation.at(2).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut("line检测");
        break;
    }
    case 308: { // circle检测
        QImage img = QOpencvProcessing::Instance()->HoughCircle(
          widget_.GetSurface(),
          imformation.at(0).toInt(),
          imformation.at(1).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("circle检测"));
        break;
    }
    case 401: { // 腐蚀
        QImage img = QOpencvProcessing::Instance()->Erode(
          widget_.GetSurface(),
          imformation.at(0).toInt(),
          imformation.at(1).toInt(),
          imformation.at(2).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("腐蚀"));
        break;
    }
    case 402: { // 膨胀
        QImage img = QOpencvProcessing::Instance()->Dilate(
          widget_.GetSurface(),
          imformation.at(0).toInt(),
          imformation.at(1).toInt(),
          imformation.at(2).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("膨胀"));
        break;
    }
    case 403: { // 开运算
        QImage img = QOpencvProcessing::Instance()->Open(
          widget_.GetSurface(),
          imformation.at(0).toInt(),
          imformation.at(1).toInt(),
          imformation.at(2).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("开运算"));
        break;
    }
    case 404: { // 闭运算
        QImage img = QOpencvProcessing::Instance()->Close(
          widget_.GetSurface(),
          imformation.at(0).toInt(),
          imformation.at(1).toInt(),
          imformation.at(2).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("闭运算"));
        break;
    }
    case 405: { // 形态学梯度
        QImage img = QOpencvProcessing::Instance()->Grad(
          widget_.GetSurface(),
          imformation.at(0).toInt(),
          imformation.at(1).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("形态学梯度"));
        break;
    }
    case 406: { // 顶帽操作
        QImage img = QOpencvProcessing::Instance()->Tophat(
          widget_.GetSurface(),
          imformation.at(0).toInt(),
          imformation.at(1).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("顶帽操作"));
        break;
    }
    case 407: { // 黑帽操作
        QImage img = QOpencvProcessing::Instance()->Blackhat(
          widget_.GetSurface(),
          imformation.at(0).toInt(),
          imformation.at(1).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("黑帽操作"));
        break;
    }
    case 501: { // 颜色模型
        QImage img = QOpencvProcessing::Instance()->splitColor(
          widget_.GetSurface(),
          imformation.at(0).toLocal8Bit().data(),
          imformation.at(1).toInt());
        UpDataImage(img);
        emit SignalPromptInformationOut(("颜色模型"));
        break;
    }
    case 601: { // 最短路径提取测试
        //        MainWindow *tmp = new MainWindow;
        //        tmp->OpenMinPathWidgetIn(widget_.GetSurface());
        break;
    }
    case 602: { // 边缘调整控件测试
        FormEdgeAdjustment *tmp = new FormEdgeAdjustment();
        tmp->showMaximized();
        break;
    }
    case 603: { // IVUS浏览控件
        ReadIVUS *tmp = new ReadIVUS();
        tmp->show();
        break;
    }
    default: {
        break;
    }
    }
}
