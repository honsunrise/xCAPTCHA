//
// Created by zhsyourai on 8/24/17.
//

#ifndef XCAPTCHA_ID_ENGINE_AFFINE_H
#define XCAPTCHA_ID_ENGINE_AFFINE_H

#include "image_deformation_engine.h"
class id_engine_affine : public image_deformation_engine {
 protected:
  Mat2d calc_delta(size_t grid_size,
                   double alpha,
                   const vector<Point2d> &p,
                   const vector<Point2d> &q,
                   const int tar_w,
                   const int tar_h) override;
};

#endif //XCAPTCHA_ID_ENGINE_AFFINE_H
