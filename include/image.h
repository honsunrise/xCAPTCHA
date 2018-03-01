//
// Created by zhsyourai on 9/4/17.
//

#ifndef XCAPTCHA_IMAGE_H
#define XCAPTCHA_IMAGE_H

#include <cstdint>
#include <opencv2/core/core.hpp>

class image {
 public:
  image();

  image(const image &other);

  image(image &&other) noexcept;

  image(int32_t rows, int32_t cols);

  explicit image(const cv::Mat &mat);

  image &operator=(const image &other);

  image &operator=(image &&other) noexcept;

  operator cv::Mat() const;

  operator cv::Mat();

  virtual ~image();

 private:
  int32_t rows;
  int32_t cols;
  int32_t channels;
  uint8_t *data;
};

#endif //XCAPTCHA_IMAGE_H
