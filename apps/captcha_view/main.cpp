#include <iostream>
#include <opencv2/opencv.hpp>
#include "captcha_context.h"

using namespace cv;
int main() {
  captcha_context cc;
  cc.load_config("config/demo.yaml");
  namedWindow("Display Image", WINDOW_AUTOSIZE );
  while(1) {
    captcha ca = cc.generate();
    imshow("Display Image", static_cast<cv::Mat>(ca.get_image()));
    if(waitKey() == 'q')
      break;
    else {

    }
  }
  return 0;
}