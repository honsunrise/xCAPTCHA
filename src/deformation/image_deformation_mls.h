#ifndef IMGTRANS_MLS_H
#define IMGTRANS_MLS_H

#include <opencv2/opencv.hpp>
#include <vector>
#include "engine/image_deformation_engine.h"
#include "engine/id_engine_affine.h"
#include "engine/id_engine_grid.h"
#include "engine/id_engine_rigid.h"
#include "engine/id_engine_similarity.h"

using std::vector;
using cv::Mat;
using cv::Mat2d;
using cv::Point;
using cv::Point2d;

class image_deformation_mls {
 public:
  image_deformation_mls(const image_deformation_engine *image_deformation,
                        double alpha,
                        size_t grid_size,
                        int src_w,
                        int src_h,
                        const vector<Point2d> &p,
                        const vector<Point2d> &q,
                        int tar_w,
                        int tar_h);

  image_deformation_mls(const image_deformation_engine *image_deformation,
                        double alpha,
                        size_t grid_size,
                        int src_w,
                        int src_h,
                        const vector<Point2d> &p,
                        const vector<Point2d> &q);

  virtual ~image_deformation_mls() {}

  Mat genNewImg(const Mat &oriImg);
 protected:
  const image_deformation_engine *image_deformation;
  size_t grid_size = 5;
  double alpha = 1;
  vector<Point2d> p, q;
  size_t n_point;
  Mat2d delta;
  int src_w, src_h;
  int tar_w, tar_h;
};

#endif  // IMGTRANS_MLS_H
