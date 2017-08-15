//
// Created by zhsyourai on 8/4/17.
//

#ifndef XCAPTCHA_CAPTCHA_H
#define XCAPTCHA_CAPTCHA_H
#include <cstdint>
#include <opencv2/opencv.hpp>

class captcha {
 public:
  captcha();
  captcha(int32_t rows, int32_t cols);
  captcha(const cv::Mat &mat);

  explicit operator cv::Mat();
  virtual ~captcha();
 private:
  int32_t rows;
  int32_t cols;
  uint8_t *data;
};

#endif //XCAPTCHA_CAPTCHA_H
