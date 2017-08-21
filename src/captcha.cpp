//
// Created by zhsyourai on 8/4/17.
//

#include "captcha.h"
captcha::captcha() : rows(0), cols(0), channels(3), data(nullptr) {
}

captcha::captcha(const captcha &other) {
  rows = other.rows;
  cols = other.cols;
  channels = other.channels;
  data = new uint8_t[rows * cols * channels];
  std::memcpy(data, other.data, static_cast<size_t>(rows * cols * channels));
}

captcha::captcha(captcha &&other) noexcept {
  rows = other.rows;
  cols = other.cols;
  data = other.data;
  other.data = nullptr;
  other.cols = other.rows = 0;
}
captcha &captcha::operator=(const captcha &other) {
  delete[] data;
  rows = other.rows;
  cols = other.cols;
  channels = other.channels;
  data = new uint8_t[rows * cols * channels];
  std::memcpy(data, other.data, static_cast<size_t>(rows * cols * channels));
  return *this;
}
captcha &captcha::operator=(captcha &&other) noexcept {
  delete[] data;
  rows = other.rows;
  cols = other.cols;
  channels = other.channels;
  data = other.data;
  other.data = nullptr;
  other.cols = other.rows = other.channels = 0;
  return *this;
}

captcha::captcha(int32_t rows, int32_t cols) : rows(rows), cols(cols), channels(3) {
  data = new uint8_t[rows * cols * channels];
}

captcha::~captcha() {
  cols = rows = channels = 0;
  delete[] data;
}

captcha::captcha(const cv::Mat &mat) {
  cols = mat.cols;
  rows = mat.rows;
  channels = mat.channels();
  data = new uint8_t[cols * rows * channels];
  if (mat.isContinuous()) {
    std::memcpy(data, mat.ptr(), static_cast<size_t>(cols * rows * channels));
  } else {
    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < cols; ++j) {
        for (int k = 0; k < channels; ++k) {
          data[i * cols * channels + j * channels + k] = mat.at<uint8_t>(i, j * channels + k);
        }
      }
    }
  }
}

captcha::operator cv::Mat() {
  cv::Mat mat(rows, cols, CV_MAKETYPE(CV_8U, channels));
  if (mat.isContinuous()) {
    std::memcpy(mat.ptr(), data, static_cast<size_t>(cols * rows * channels));
  } else {
    for (int i = 0; i < rows; ++i) {
      auto *ptrDst = mat.ptr<uint8_t>(i);
      for (int j = 0; j < cols; ++j) {
        for (int k = 0; k < channels; ++k) {
          ptrDst[j * channels + k] = data[i * cols * channels + j * channels + k];
        }
      }
    }
  }
  return mat;
}
