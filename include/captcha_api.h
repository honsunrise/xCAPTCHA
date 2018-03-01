//
// Created by zhsyourai on 8/7/17.
//

#ifndef XCAPTCHA_CAPTCHA_API_H
#define XCAPTCHA_CAPTCHA_API_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <locale>

using cv::Mat;
using cv::Point2d;
using std::vector;

enum deformation_type : char {
  AFFINE, GRID, RIGID, SIMILARITY
};

class captcha_api {
 public:
  bool get_env(std::string key) const;

  Mat deformation(deformation_type type,
                  const vector<Point2d> &q,
                  const vector<Point2d> &p,
                  double alpha,
                  size_t grid_size,
                  const Mat &src) const;

  uint32_t random_char(std::locale locale) const;
};

#endif //XCAPTCHA_CAPTCHA_API_H
