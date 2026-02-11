#ifndef QOPENCVPROCESSING_H
#define QOPENCVPROCESSING_H

/**
 * qt封装
 * opencv 常用函数 输入输出参数转为qimage
 */

// 01 Frame includes
#include "stable.h"

// OPENCV includes
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>



class QOpencvProcessing {
  public:
    QOpencvProcessing();
    ~QOpencvProcessing();
    static QOpencvProcessing *Instance();
    // 图像转换
    QImage cvMat2QImage(const cv::Mat &mat);// Mat 改成 QImage
    cv::Mat QImage2cvMat(QImage image);// QImage 改成 Mat
    QImage splitBGR(QImage src, int color);	// 提取RGB分量
    QImage splitColor(QImage src, cv::String model, int color);// 提取分量
    // 图像几何变换
    void Resize(QImage &src, int length, int width);// 改变大小
    void Enlarge_Reduce(QImage &src, int times);// 缩放
    void Rotate(QImage &src, int angle);// 旋转
    void Rotate_fixed(QImage &src, int angle);// 旋转90，180，270
    void Flip(QImage &src, int flipcode);// 镜像
    void Lean(QImage &src, int x, int y);// 倾斜
    // 图像增强
    QImage Normalized(QImage src, int kernel_length);// 简单滤波
    QImage Gaussian(QImage src, int kernel_length);	// 高斯滤波
    QImage Median(QImage src, int kernel_length);// 中值滤波
    QImage Sobel(QImage src, int kernel_length);// sobel边缘检测
    QImage Laplacian(QImage src, int kernel_length);// laplacian边缘检测
    QImage Canny(QImage src, int kernel_length, int lowThreshold);// canny边缘检测
    QImage HoughLine(
        QImage src, int threshold, double minLineLength, double maxLineGap);// 线检测
    QImage HoughCircle(QImage src, int minRadius, int maxRadius);// 圆检测
    // 灰度变化
    QImage Bin(QImage src, int threshold);
    QImage Graylevel(QImage src);
    QImage Reverse(QImage src);								// 图像反转
    QImage Linear(QImage src, int alpha, int beta);			// 线性变换
    QImage Gamma(QImage src, int gamma);					// 伽马变换(指数变换)
    QImage Log(QImage src, int c);							// 对数变换
    QImage Histeq(QImage src);								// 直方图均衡化
    void BinToGraylevel(cv::Mat &image);
    bool IsBin(cv::Mat &image);
    // 图像腐蚀
    QImage Erode(QImage src, int elem, int kernel, int times);// 腐蚀
    QImage Dilate(QImage src, int elem, int kernel, int times);// 膨胀
    QImage Open(QImage src, int elem, int kernel, int times);// 开运算
    QImage Close(QImage src, int elem, int kernel, int times);// 闭运算
    QImage Grad(QImage src, int elem, int kernel);// 形态学梯度
    QImage Tophat(QImage src, int elem, int kernel);// 顶帽操作
    QImage Blackhat(QImage src, int elem, int kernel);// 黑帽操作
  private:
    static QScopedPointer<QOpencvProcessing> self;
};
#endif // QOPENCVPROCESSING_H
