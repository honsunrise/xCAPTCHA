//
// Created by zhsyourai on 8/25/17.
//

#include "captcha_api.h"
#include "deformation/image_deformation_mls.h"

bool captcha_api::get_env(std::string key) const {
  return false;
}
Mat captcha_api::deformation(deformation_type type, const vector<Point2d> &q, const vector<Point2d> &p, const double alpha,
                             size_t grid_size, const Mat &src) const {
  image_deformation_engine *engine = nullptr;
  switch (type) {
    case AFFINE:
      engine = new id_engine_affine();
      break;
    case GRID:
      engine = new id_engine_grid();
      break;
    case RIGID:
      engine = new id_engine_rigid();
      break;
    case SIMILARITY:
      engine = new id_engine_similarity();
      break;
  }
  image_deformation_mls mls(engine, alpha, grid_size, src.cols, src.rows, q, p);
  return mls.genNewImg(src);
}
