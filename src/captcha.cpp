//
// Created by zhsyourai on 8/4/17.
//

#include "captcha.h"
captcha::captcha(): rows(0), cols(0), data(nullptr) {}

captcha::captcha(int32_t rows, int32_t cols) : rows(rows), cols(cols) {
  data = new uint8_t[rows * cols * 4];
}

captcha::~captcha() {
  cols = rows = 0;
  delete [] data;
}

captcha::captcha(const cv::Mat &mat) {
  data = new uint8_t[mat.cols * mat.rows * 4];
  if(mat.isContinuous()) {
    const auto *ptrDst = mat.ptr<uint8_t>();
    for(int i = 0; i < mat.total(); ++i) {
      data[i] = ptrDst[i];
    }
  } else {
    for(int i = 0; i < mat.rows; ++i) {
      const auto *ptrDst = mat.ptr<uint8_t>(i);
      for(int j = 0; j < mat.cols; ++j) {
        data[i * mat.cols + j] = ptrDst[j];
      }
    }
  }
}

captcha::operator cv::Mat() {
  cv::Mat mat(rows, cols, CV_8UC4);
  for (int i = 0; i < rows; ++i) {
    auto *ptrDst = mat.ptr<uint8_t>(i);
    for (int j = 0; j < cols; ++j) {
      ptrDst[j] = data[i * mat.cols + j];
    }
  }
  return mat;
}
