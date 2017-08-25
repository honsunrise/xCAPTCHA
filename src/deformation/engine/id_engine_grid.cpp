//
// Created by zhsyourai on 8/25/17.
//

#include "delaunay.h"
#include "id_engine_grid.h"
using cv::Mat1i;
using cv::Mat1b;
using cv::Vec2d;
Mat2d id_engine_grid::calc_delta(const int src_w,
                                 const int src_h,
                                 size_t grid_size,
                                 double alpha,
                                 const vector<Point2d> &p,
                                 const vector<Point2d> &q,
                                 const int tar_w,
                                 const int tar_h) const {
  assert(p.size() == q.size());
  size_t n_point = p.size();
  vector<Point2d> new_p = p;
  vector<Point2d> new_q = q;
  Mat1i img_mask = Mat1i::zeros(tar_h, tar_w);
  Mat2d delta = Mat2d::zeros(tar_h, tar_w);
  for (int i = 0; i < n_point; i++) {
    if (new_p[i].x < 0) new_p[i].x = 0;
    if (new_p[i].y < 0) new_p[i].y = 0;
    if (new_p[i].x >= tar_w) new_p[i].x = tar_w - 1;
    if (new_p[i].y >= tar_h) new_p[i].y = tar_h - 1;

    if (new_q[i].x < 0) new_q[i].x = 0;
    if (new_q[i].y < 0) new_q[i].y = 0;
    if (new_q[i].x >= tar_w) new_q[i].x = tar_w - 1;
    if (new_q[i].y >= tar_h) new_q[i].y = tar_h - 1;

    delta(new_p[i])[0] = new_q[i].x - new_p[i].x;
    delta(new_p[i])[1] = new_q[i].y - new_p[i].y;
  }
  cv::Mat_<uchar> imgTmpP = cv::Mat_<uchar>::zeros(tar_h, tar_w);
  for (const auto &it:new_p) {
    cv::circle(imgTmpP, it, 1, 255, 1, CV_AA);
  }
  imshow("imgTmpP", imgTmpP);

  cv::Mat_<uchar> imgTmpQ = cv::Mat_<uchar>::zeros(tar_h, tar_w);
  for (const auto &it:new_q) {
    cv::circle(imgTmpQ, it, 1, 255, 1, CV_AA);
  }
  imshow("imgTmpQ", imgTmpQ);

  delta(0, 0) = Vec2d(0, 0);
  delta(tar_h - 1, 0) = Vec2d(0, src_h - tar_h);
  delta(0, tar_w - 1) = Vec2d(0, src_w - tar_w);
  delta(tar_h - 1, tar_w - 1) = Vec2d(src_w - tar_w, src_h - tar_h);
  cv::Rect bound_rect(0, 0, tar_w, tar_h);
//  new_p.emplace_back(0, 0);
//  new_p.emplace_back(0, tar_h - 1);
//  new_p.emplace_back(tar_w - 1, 0);
//  new_p.emplace_back(tar_w - 1, tar_h - 1);
  vector<Triangle> triangles = ::delaunayDiv(new_p, bound_rect);
  cv::Mat_<uchar> imgTmp = cv::Mat_<uchar>::zeros(tar_h, tar_w);
  for (auto it = triangles.begin(); it != triangles.end(); it++) {
    cv::line(imgTmp, it->v[0], it->v[1], 255, 1, CV_AA);
    cv::line(imgTmp, it->v[0], it->v[2], 255, 1, CV_AA);
    cv::line(imgTmp, it->v[2], it->v[1], 255, 1, CV_AA);
    if (!(it->v[0].inside(bound_rect) && it->v[1].inside(bound_rect) && it->v[2].inside(bound_rect)))
      continue;

    cv::fillConvexPoly(img_mask, it->v, 3, cv::Scalar(it - triangles.begin() + 1));
  }
  imshow("imgTmp", imgTmp);
  cvWaitKey(10);

  Point2i v1, v2, curV;
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
      int tId = img_mask(j, i) - 1;
      if (tId < 0) {
        delta(j, i) = Vec2d(-i, -j);
        continue;
      }
      v1 = triangles[tId].v[1] - triangles[tId].v[0];
      v2 = triangles[tId].v[2] - triangles[tId].v[0];
      curV.x = i, curV.y = j;
      curV -= triangles[tId].v[0];

      double d0, d1, d2;
      d2 = double(v1.x * curV.y - curV.x * v1.y) / (v1.x * v2.y - v2.x * v1.y);
      d1 = double(v2.x * curV.y - curV.x * v2.y) / (v2.x * v1.y - v1.x * v2.y);
      d0 = 1 - d1 - d2;
      delta(j, i) =
          Vec2d(d0 * delta(triangles[tId].v[0])[0] +
                    d1 * delta(triangles[tId].v[1])[0] +
                    d2 * delta(triangles[tId].v[2])[0],
                d0 * delta(triangles[tId].v[0])[1] +
                    d1 * delta(triangles[tId].v[1])[1] +
                    d2 * delta(triangles[tId].v[2])[1]);
    }
  }
  return delta;
}
