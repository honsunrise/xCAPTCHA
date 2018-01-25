//
// Created by zhsyourai on 8/4/17.
//

#include <random>
#include "filling_plugin.h"

void filling_plugin::initialization(const captcha_api &api) {
  this->api = &api;
}

void filling_plugin::release() {

}

captcha_config::config_define filling_plugin::get_config_define() const {
  captcha_config::config_define define;
  define["color"] = captcha_config::config_define();
  define["color"]["r"] = -1;
  define["color"]["g"] = -1;
  define["color"]["b"] = -1;
  define["color"]["random"] = captcha_config::config_define();
  define["color"]["random"]["min"] = 0U;
  define["color"]["random"]["max"] = 256U;
  return define;
}

void filling_plugin::set_config(const captcha_config::config &node) {
  const auto &color = node["color"];
  r = color["r"];
  g = color["g"];
  b = color["b"];

  const auto &random = node["color"]["random"];
  random_min = random["min"];
  random_max = random["max"];
}

void filling_plugin::pipe(captcha &in) const {
  image &img = in.get_image();
  static std::random_device rd;
  static auto
      dice =
      std::bind(std::uniform_int_distribution<int32_t>(random_min, random_max), std::default_random_engine(rd()));
  cv::Mat mat = img;
  int32_t t_r = r > 0 ? r : dice();
  int32_t t_g = g > 0 ? g : dice();
  int32_t t_b = b > 0 ? b : dice();
  const cv::Scalar &color = cv::Scalar(t_b, t_g, t_r);
  cv::rectangle(mat,
                cv::Point(0, 0),
                cv::Point(mat.cols, mat.rows),
                color, cv::FILLED, cv::LINE_8);
  img = image(mat);
}

processor_plugin_interface *create() {
  return new filling_plugin();
}

void destroy(processor_plugin_interface *plugin_interface) {
  delete plugin_interface;
}
