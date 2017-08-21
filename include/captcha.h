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
  captcha(const captcha &other);
  captcha(captcha &&other) noexcept;

  captcha(int32_t rows, int32_t cols);
  explicit captcha(const cv::Mat &mat);

  captcha &operator=(const captcha &other);
  captcha &operator=(captcha &&other) noexcept;

  explicit operator cv::Mat();
  virtual ~captcha();
 private:
  int32_t rows;
  int32_t cols;
  int32_t channels;
  uint8_t *data;
};

#endif //XCAPTCHA_CAPTCHA_H
