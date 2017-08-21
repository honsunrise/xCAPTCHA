//
// Created by zhsyourai on 8/4/17.
//

#include "noise_plugin.h"

void noise_plugin::initialization(const captcha_api &api) {
  this->api = &api;
}

void noise_plugin::release() {
  this->api = nullptr;
}

captcha_config::config_define noise_plugin::get_config_define() const {
  captcha_config::config_define define;
  define["color"] = captcha_config::config_define();
  define["color"]["r"] = -1;
  define["color"]["g"] = -1;
  define["color"]["b"] = -1;
  define["color"]["random"] = captcha_config::config_define();
  define["color"]["random"]["min"] = 0U;
  define["color"]["random"]["max"] = 256U;

  define["type"] = "line";
  return define;
}

void noise_plugin::set_config(const captcha_config::config &node) {
  const auto &color = node["color"];
  r = color["r"];
  g = color["g"];
  b = color["b"];

  const auto &random = node["color"]["random"];
  random_min = random["min"];
  random_max = random["max"];

  type = node["type"];
}

void MyLine(cv::Mat &img, const cv::Scalar &color, cv::Point start, cv::Point end) {
  int thickness = 1;
  int lineType = cv::LINE_AA;
  cv::line(img,
           start,
           end,
           color,
           thickness,
           lineType);
}

captcha noise_plugin::pipe(captcha &in) {
  static auto
      dice = std::bind(std::uniform_int_distribution<int32_t>(random_min, random_max), std::default_random_engine());
  cv::Mat image = static_cast<cv::Mat>(in);
  int32_t t_r = r > 0 ? r : dice();
  int32_t t_g = g > 0 ? g : dice();
  int32_t t_b = b > 0 ? b : dice();
  MyLine(image, cv::Scalar(t_b, t_g, t_r), cv::Point(0,0), cv::Point(100,100));
  return captcha(image);
}

processor_plugin_interface *create() {
  return new noise_plugin();
}

void destroy(processor_plugin_interface *plugin_interface) {
  delete plugin_interface;
}
