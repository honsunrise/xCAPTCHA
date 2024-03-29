//
// Created by zhsyourai on 8/24/17.
//

#include "id_engine_rigid.h"

Mat2d id_engine_rigid::calc_delta(const int src_w,
                                  const int src_h,
                                  size_t grid_size,
                                  double alpha,
                                  const vector<Point2d> &p,
                                  const vector<Point2d> &q,
                                  const int tar_w,
                                  const int tar_h) const {
  assert(p.size() == q.size());
  size_t n_point = p.size();

  Point2d swq, qstar, newP, tmpP;
  auto *w = new double[n_point];
  double sum_w = 0;

  Mat2d delta(tar_h, tar_w);
  if (n_point < 2) {
    delta.setTo(0);
    return delta;
  }

  Point2d swp, pstar, curV, curVJ, Pi, PiJ, Qi;
  double miu_r;

  for (int i = 0;; i += grid_size) {
    if (i >= tar_w && i < tar_w + grid_size - 1)
      i = tar_w - 1;
    else if (i >= tar_w)
      break;
    for (int j = 0;; j += grid_size) {
      if (j >= tar_h && j < tar_h + grid_size - 1)
        j = tar_h - 1;
      else if (j >= tar_h)
        break;
      sum_w = 0;
      swp.x = swp.y = 0;
      swq.x = swq.y = 0;
      newP.x = newP.y = 0;
      curV.x = i;
      curV.y = j;
      int k;
      for (k = 0; k < n_point; k++) {
        if ((i == p[k].x) && j == p[k].y) break;
        if (alpha == 1)
          w[k] = 1 / ((i - p[k].x) * (i - p[k].x) +
              (j - p[k].y) * (j - p[k].y));
        else
          w[k] = pow((i - p[k].x) * (i - p[k].x) +
              (j - p[k].y) * (j - p[k].y), -alpha);
        sum_w = sum_w + w[k];
        swp = swp + w[k] * p[k];
        swq = swq + w[k] * q[k];
      }
      if (k == n_point) {
        pstar = 1 / sum_w * swp;
        qstar = 1 / sum_w * swq;

        // Calc miu_r
        double s1 = 0, s2 = 0;
        for (k = 0; k < n_point; k++) {
          if (i == p[k].x && j == p[k].y) continue;

          Pi = p[k] - pstar;
          PiJ.x = -Pi.y, PiJ.y = Pi.x;
          Qi = q[k] - qstar;
          s1 += w[k] * Qi.dot(Pi);
          s2 += w[k] * Qi.dot(PiJ);
        }
        miu_r = sqrt(s1 * s1 + s2 * s2);

        curV -= pstar;
        curVJ.x = -curV.y, curVJ.y = curV.x;

        for (k = 0; k < n_point; k++) {
          if (i == p[k].x && j == p[k].y) continue;

          Pi = p[k] - pstar;
          PiJ.x = -Pi.y, PiJ.y = Pi.x;

          tmpP.x = Pi.dot(curV) * q[k].x -
              PiJ.dot(curV) * q[k].y;
          tmpP.y = -Pi.dot(curVJ) * q[k].x +
              PiJ.dot(curVJ) * q[k].y;
          tmpP *= w[k] / miu_r;
          newP += tmpP;
        }
        newP += qstar;
      } else {
        newP = q[k];
      }
      delta.at<cv::Vec2d>(j, i)[0] = newP.x - i;
      delta.at<cv::Vec2d>(j, i)[1] = newP.y - j;
    }
  }
  delete[] w;
  return delta;
}
