//
// Created by zhsyourai on 8/4/17.
//

#include "background_plugin.h"

void background_plugin::initialization(const captcha_api &api) {
  this->api = &api;
}

void background_plugin::release() {

}

captcha_config::config_define background_plugin::get_config_define() const {
  captcha_config::config_define define;
  define["image_dir"] = captcha_config::config_define();
  define["color"]["r"] = -1;
  define["color"]["g"] = -1;
  define["color"]["b"] = -1;
  define["color"]["random"] = captcha_config::config_define();
  define["color"]["random"]["min"] = 0U;
  define["color"]["random"]["max"] = 256U;
  return define;
}

void background_plugin::set_config(const captcha_config::config &node) {
  const auto &color = node["color"];
  r = color["r"];
  g = color["g"];
  b = color["b"];

  const auto &random = node["color"]["random"];
  random_min = random["min"];
  random_max = random["max"];
}

captcha background_plugin::pipe(captcha &in) {
  static std::random_device rd;
  static auto
      dice = std::bind(std::uniform_int_distribution<int32_t>(random_min, random_max), std::default_random_engine(rd()));
  cv::Mat image = in;
  int32_t t_r = r > 0 ? r : dice();
  int32_t t_g = g > 0 ? g : dice();
  int32_t t_b = b > 0 ? b : dice();
  const cv::Scalar &color = cv::Scalar(t_b, t_g, t_r);
  cv::rectangle(image,
                cv::Point(0, 0),
                cv::Point(image.cols, image.rows),
                color, cv::FILLED, cv::LINE_8);
  return captcha(image);
}

processor_plugin_interface *create() {
  return new background_plugin();
}

void destroy(processor_plugin_interface *plugin_interface) {
  delete plugin_interface;
}
