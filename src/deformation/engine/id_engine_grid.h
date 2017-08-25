//
// Created by zhsyourai on 8/25/17.
//

#ifndef XCAPTCHA_ID_ENGINE_GRID_H
#define XCAPTCHA_ID_ENGINE_GRID_H

#include "image_deformation_engine.h"
class id_engine_grid : public image_deformation_engine {
 protected:
  Mat2d calc_delta(const int src_w,
                   const int src_h,
                   size_t grid_size,
                   double alpha,
                   const vector<Point2d> &p,
                   const vector<Point2d> &q,
                   const int tar_w,
                   const int tar_h) const override;
};

#endif //XCAPTCHA_ID_ENGINE_GRID_H
