//
// Created by zhsyourai on 8/17/17.
//

#include <random>
#include "deformation_plugin.h"

void deformation_plugin::initialization(const captcha_api &api) {
  this->api = &api;
}

void deformation_plugin::release() {

}

config_define deformation_plugin::get_config_define() const {
  return nullptr;
}

void deformation_plugin::set_config(const captcha_config::config &node) {

}

void deformation_plugin::pipe(captcha &in) const {
  static std::random_device rd;
  static auto dice = std::bind(std::uniform_int_distribution<int32_t>(-5, 5), std::default_random_engine(rd()));
  image &img = in.get_image();
  cv::Mat mat = img;
  vector<Point2d> q;
  vector<Point2d> p;
  const size_t grid_size = 20;
  for (int j = 0; j <= mat.rows; j += grid_size) {
    if (j >= mat.rows)
      j = mat.rows - 1;
    for (int i = 0; i <= mat.cols; i += grid_size) {
      if (i >= mat.cols)
        i = mat.cols - 1;
      q.emplace_back(i, j);
    }
  }
  p = q;
  for (auto &it : p) {
    it.x += dice();
    it.y += dice();
  }
  cv::Mat ret = api->deformation(deformation_type::RIGID, q, p, 1, grid_size, mat);
  img = image(ret);
}

processor_plugin_interface *create() {
  return new deformation_plugin();
}

void destroy(processor_plugin_interface *plugin_interface) {
  delete plugin_interface;
}