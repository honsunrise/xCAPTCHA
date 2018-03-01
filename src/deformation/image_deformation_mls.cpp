#include "image_deformation_mls.h"

using cv::Vec3b;

inline double bilinear_interp(double x, double y, double v11, double v12,
                              double v21, double v22) {
  return (v11 * (1 - y) + v12 * y) * (1 - x) + (v21 * (1 - y) + v22 * y) * x;
}

image_deformation_mls::image_deformation_mls(const image_deformation_engine *image_deformation,
                                             const double alpha,
                                             const size_t grid_size,
                                             const int src_w,
                                             const int src_h,
                                             const vector<Point2d> &q,
                                             const vector<Point2d> &p,
                                             const int tar_w,
                                             const int tar_h)
    : image_deformation(image_deformation),
      src_w(src_w),
      src_h(src_h),
      tar_w(tar_w),
      tar_h(tar_h),
      grid_size(grid_size) {
  assert(p.size() == q.size());
  this->delta = (*this->image_deformation)(src_w, src_h, grid_size, alpha, q, p, tar_w, tar_h);
}

image_deformation_mls::image_deformation_mls(const image_deformation_engine *image_deformation,
                                             const double alpha,
                                             const size_t grid_size,
                                             const int src_w,
                                             const int src_h,
                                             const vector<Point2d> &q,
                                             const vector<Point2d> &p)
    : image_deformation(image_deformation),
      src_w(src_w),
      src_h(src_h),
      tar_w(src_w),
      tar_h(src_h),
      grid_size(grid_size) {
  assert(p.size() == q.size());
  this->delta = (*this->image_deformation)(src_w, src_h, grid_size, alpha, q, p, tar_w, tar_h);
}

Mat image_deformation_mls::genNewImg(const Mat &oriImg) {
  Mat newImg(tar_h, tar_w, oriImg.type());
  for (int i = 0; i < tar_h; i += grid_size) {
    for (int j = 0; j < tar_w; j += grid_size) {
      int ni = i + grid_size, nj = j + grid_size;
      double w = grid_size, h = grid_size;
      if (ni >= tar_h) {
        ni = tar_h - 1;
        h = ni - i + 1;
      }
      if (nj >= tar_w) {
        nj = tar_w - 1;
        w = nj - j + 1;
      }
      for (int di = 0; di < h; di++) {
        for (int dj = 0; dj < w; dj++) {
          double deltaX =
              bilinear_interp(di / h,
                              dj / w,
                              delta.at<cv::Vec2d>(i, j)[0],
                              delta.at<cv::Vec2d>(i, nj)[0],
                              delta.at<cv::Vec2d>(ni, j)[0],
                              delta.at<cv::Vec2d>(ni, nj)[0]);
          double deltaY =
              bilinear_interp(di / h * 1.0,
                              dj / w * 1.0,
                              delta.at<cv::Vec2d>(i, j)[1],
                              delta.at<cv::Vec2d>(i, nj)[1],
                              delta.at<cv::Vec2d>(ni, j)[1],
                              delta.at<cv::Vec2d>(ni, nj)[1]);
          double nx = j + dj + deltaX;
          double ny = i + di + deltaY;
          if (nx > src_w - 1) nx = src_w - 1;
          if (ny > src_h - 1) ny = src_h - 1;
          if (nx < 0) nx = 0;
          if (ny < 0) ny = 0;
          auto nxi = int(nx), nyi = int(ny);
          int nxi1 = ceil(nx), nyi1 = ceil(ny);
          if (oriImg.channels() == 1) {
            newImg.at<uchar>(i + di, j + dj) = static_cast<uchar>(bilinear_interp(
                ny - nyi, nx - nxi,
                oriImg.at<uchar>(nyi, nxi),
                oriImg.at<uchar>(nyi, nxi1),
                oriImg.at<uchar>(nyi1, nxi),
                oriImg.at<uchar>(nyi1, nxi1)));
          } else {
            for (int k = 0; k < 3; ++k) {
              newImg.at<Vec3b>(i + di, j + dj)[k] = static_cast<uchar>(bilinear_interp(
                  ny - nyi, nx - nxi,
                  oriImg.at<Vec3b>(nyi, nxi)[k],
                  oriImg.at<Vec3b>(nyi, nxi1)[k],
                  oriImg.at<Vec3b>(nyi1, nxi)[k],
                  oriImg.at<Vec3b>(nyi1, nxi1)[k]));
            }
          }
        }
      }
    }
  }
  return newImg;
}

image_deformation_mls::image_deformation_mls(const Mat2d &delta, int src_w, int src_h, int tar_w, int tar_h) : delta(
    delta), src_w(src_w), src_h(src_h), tar_w(tar_w), tar_h(tar_h) {}
