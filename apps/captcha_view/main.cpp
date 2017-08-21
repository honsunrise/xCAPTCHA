#include <iostream>
#include <opencv2/opencv.hpp>
#include "captcha_context.h"

using namespace cv;
int main() {
  captcha_context cc;
  cc.load_config("config/demo.yaml");
  namedWindow("Display Image", WINDOW_AUTOSIZE );
  while(1) {
    Mat image = cc.generate();
    imshow("Display Image", image);
    if(waitKey(500) == 'q')
      break;
  }
  return 0;
}