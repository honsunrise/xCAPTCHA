#ifndef IMGTRANS_MLS_H
#define IMGTRANS_MLS_H

#include <opencv2/opencv.hpp>
#include <vector>
using std::vector;
using cv::Mat;
using cv::Mat2d;
using cv::Point;
using cv::Point2d;

class image_deformation_mls {
 public:
  image_deformation_mls(const int src_w,
                        const int src_h,
                        const vector<Point2d> &p,
                        const vector<Point2d> &q,
                        const int tar_w,
                        const int tar_h);

  image_deformation_mls(const int src_w,
                        const int src_h,
                        const vector<Point2d> &p,
                        const vector<Point2d> &q);

  virtual ~image_deformation_mls() {}

  Mat genNewImg(const Mat &oriImg);
 protected:
  int grid_size = 5;
  double alpha = 1;
  vector<Point2d> p, q;
  size_t n_point;
  Mat2d delta;
  int src_w, src_h;
  int tar_w, tar_h;
};

#endif  // IMGTRANS_MLS_H
