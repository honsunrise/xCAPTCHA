//
// Created by zhsyourai on 8/4/17.
//

#include "filling_plugin.h"

void filling_plugin::initialization(const captcha_api &api) {

}

void filling_plugin::release() {

}

captcha_config::config_define filling_plugin::get_config_define() const {
  captcha_config::config_define config_define(captcha_config::config_define::config_define_node_type::CONTAINER);
  config_define.insert("color", new captcha_config::config_define(0));
  return config_define;
}

void filling_plugin::set_config(const captcha_config::config &node) {

}

captcha filling_plugin::pipe(captcha &in) {
  cv::Mat image = static_cast<cv::Mat>(in);
  cv::rectangle(image,
                cv::Point(0, 0),
                cv::Point(image.cols, image.rows),
                cv::Scalar(255, 0, 0), -1, 8);
  return captcha(image);
}

processor_plugin_interface *create() {
  return new filling_plugin();
}

void destroy(processor_plugin_interface *plugin_interface) {
  delete plugin_interface;
}
