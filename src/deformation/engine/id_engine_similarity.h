//
// Created by zhsyourai on 8/24/17.
//

#ifndef XCAPTCHA_ID_ENGINE_SIMILARITY_H
#define XCAPTCHA_ID_ENGINE_SIMILARITY_H

#include <vector>
#include "image_deformation_engine.h"
using std::vector;

class id_engine_similarity : public image_deformation_engine {
 protected:
  Mat2d calc_delta(const int src_w,
                   const int src_h,
                   size_t grid_size,
                   double alpha,
                   const vector<Point2d> &p,
                   const vector<Point2d> &q,
                   const int tar_w,
                   const int tar_h) override;
};

#endif //XCAPTCHA_ID_ENGINE_SIMILARITY_H
