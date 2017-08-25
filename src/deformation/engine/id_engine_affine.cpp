//
// Created by zhsyourai on 8/24/17.
//

#include "id_engine_affine.h"
Mat2d id_engine_affine::calc_delta(const int src_w,
                                   const int src_h,
                                   size_t grid_size,
                                   double alpha,
                                   const vector<Point2d> &p,
                                   const vector<Point2d> &q,
                                   const int tar_w,
                                   const int tar_h) const {
  Mat2d delta = Mat2d::zeros(tar_h, tar_w);
  return delta;
}
