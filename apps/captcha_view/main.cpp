#include <iostream>
#include <opencv2/opencv.hpp>
#include "captcha_context.h"

using namespace cv;
int main() {
  captcha_context cc;
  cc.load_config("config/demo.yaml");
  captcha cap = cc.generate();
  Mat image = static_cast<Mat>(cap);
  namedWindow("Display Image", WINDOW_AUTOSIZE );
  imshow("Display Image", image);
  waitKey(0);
  return 0;
}