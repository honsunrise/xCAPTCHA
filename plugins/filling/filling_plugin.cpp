//
// Created by zhsyourai on 8/4/17.
//

#include "filling_plugin.h"

void filling_plugin::initialization(const captcha_api &api) {

}

void filling_plugin::release() {

}

captcha_config::config_define filling_plugin::get_config_define() const {
  captcha_config::config_define color(captcha_config::config_define::config_define_node_type::CONTAINER);
  captcha_config::config_define *rgb =
      new captcha_config::config_define(captcha_config::config_define::config_define_node_type::CONTAINER);
  rgb->insert("r", new captcha_config::config_define(uint32_t(0),
                                                captcha_config::rules::minimum(0),
                                                captcha_config::rules::maximum(255)));
  rgb->insert("g", new captcha_config::config_define(uint32_t(0),
                                                captcha_config::rules::minimum(0),
                                                captcha_config::rules::maximum(255)));
  rgb->insert("b", new captcha_config::config_define(uint32_t(0),
                                                captcha_config::rules::minimum(0),
                                                captcha_config::rules::maximum(255)));
  color.insert("color", rgb);
  return color;
}

void filling_plugin::set_config(const captcha_config::config &node) {
  const captcha_config::config &color = node["color"];
  r = color["r"].as<uint32_t>();
  g = color["g"].as<uint32_t>();
  b = color["b"].as<uint32_t>();
}

captcha filling_plugin::pipe(captcha &in) {
  cv::Mat image = static_cast<cv::Mat>(in);
  cv::rectangle(image,
                cv::Point(0, 0),
                cv::Point(image.cols, image.rows),
                cv::Scalar(b, g, r), -1, 8);
  return captcha(image);
}

processor_plugin_interface *create() {
  return new filling_plugin();
}

void destroy(processor_plugin_interface *plugin_interface) {
  delete plugin_interface;
}
