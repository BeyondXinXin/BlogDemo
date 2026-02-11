#ifndef ANISOTROPICFILTERING_H
#define ANISOTROPICFILTERING_H

#include "stable.h"

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;



class AnisotropicFiltering {
  public:
    static void run();
    static void anisotropy_demo(Mat &image, Mat &result);
  private:

    static float k_;
    static float lambda_;
    static qint32 n_;
};

#endif // ANISOTROPICFILTERING_H
