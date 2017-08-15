//
// Created by zhsyourai on 8/4/17.
//

#include "captcha.h"
captcha::captcha(): rows(0), cols(0), data(nullptr) {
}

captcha::captcha(const captcha &other) {
  rows = other.rows;
  cols = other.cols;
  data = new uint32_t[rows * cols];
  std::memcpy(data, other.data, static_cast<size_t>(rows * cols));
}

captcha::captcha(captcha &&other) noexcept {
  rows = other.rows;
  cols = other.cols;
  data = other.data;
  other.data = nullptr;
  other.cols = other.rows = 0;
}
captcha &captcha::operator=(const captcha &other) {
  delete [] data;
  rows = other.rows;
  cols = other.cols;
  data = new uint32_t[rows * cols];
  std::memcpy(data, other.data, static_cast<size_t>(rows * cols));
  return *this;
}
captcha &captcha::operator=(captcha &&other) noexcept {
  delete [] data;
  rows = other.rows;
  cols = other.cols;
  data = other.data;
  other.data = nullptr;
  other.cols = other.rows = 0;
  return *this;
}

captcha::captcha(int32_t rows, int32_t cols) : rows(rows), cols(cols) {
  data = new uint32_t[rows * cols];
}

captcha::~captcha() {
  cols = rows = 0;
  delete [] data;
}

captcha::captcha(const cv::Mat &mat) {
  cv::Mat tmp;
  mat.convertTo(tmp, CV_8UC4);
  cols = tmp.cols;
  rows = tmp.rows;
  data = new uint32_t[tmp.cols * tmp.rows];
  if(tmp.isContinuous()) {
    const auto *ptrDst = tmp.ptr<uint32_t>();
    for(int i = 0; i < tmp.total(); ++i) {
      data[i] = ptrDst[i];
    }
  } else {
    for(int i = 0; i < tmp.rows; ++i) {
      const auto *ptrDst = tmp.ptr<uint32_t>(i);
      for(int j = 0; j < tmp.cols; ++j) {
        data[i * tmp.cols + j] = ptrDst[j];
      }
    }
  }
}

captcha::operator cv::Mat() {
  cv::Mat mat(rows, cols, CV_8UC4);
  for (int i = 0; i < rows; ++i) {
    auto *ptrDst = mat.ptr<uint32_t>(i);
    for (int j = 0; j < cols; ++j) {
        ptrDst[j] = data[i * cols + j];
    }
  }
  return mat;
}
