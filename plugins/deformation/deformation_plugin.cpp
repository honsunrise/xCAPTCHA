//
// Created by zhsyourai on 8/17/17.
//

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
captcha deformation_plugin::pipe(captcha &in) {
  static std::random_device rd;
  static auto
      dice = std::bind(std::uniform_int_distribution<int32_t>(-5, 5), std::default_random_engine(rd()));
  cv::Mat image = in;
  vector<Point2d> q;
  vector<Point2d> p;
  const size_t grid_size = 20;
  for (int j = 0; j <= image.rows; j += grid_size) {
    if (j >= image.rows)
      j = image.rows - 1;
    for (int i = 0; i <= image.cols; i += grid_size) {
      if (i >= image.cols)
        i = image.cols - 1;
      q.emplace_back(i, j);
    }
  }
  p = q;
  for (auto &it : p) {
    it.x += dice();
    it.y += dice();
  }
  cv::Mat ret = api->deformation(deformation_type::RIGID, q, p, 1, grid_size, image);
  return captcha(ret);
}

processor_plugin_interface *create() {
  return new deformation_plugin();
}

void destroy(processor_plugin_interface *plugin_interface) {
  delete plugin_interface;
}