//
// Created by zhsyourai on 8/24/17.
//

#ifndef XCAPTCHA_IMAGE_DEFORMATION_ENGINE_H
#define XCAPTCHA_IMAGE_DEFORMATION_ENGINE_H
#include <vector>
#include <opencv2/opencv.hpp>
using cv::Mat2d;
using cv::Point2d;
using std::vector;

class image_deformation_engine {
 public:
  inline Mat2d operator()(
      const int src_w,
      const int src_h,
      size_t grid_size,
      double alpha,
      const vector<Point2d> &p,
      const vector<Point2d> &q,
      const int tar_w,
      const int tar_h) {
    return calc_delta(src_w, src_h, grid_size, alpha, p, q, tar_w, tar_h);
  };

 protected:
  virtual Mat2d calc_delta(
      const int src_w,
      const int src_h,
      size_t grid_size,
      double alpha,
      const vector<Point2d> &p,
      const vector<Point2d> &q,
      const int tar_w,
      const int tar_h) = 0;
};
#endif //XCAPTCHA_IMAGE_DEFORMATION_ENGINE_H
