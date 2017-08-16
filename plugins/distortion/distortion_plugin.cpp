//
// Created by zhsyourai on 8/17/17.
//

#include "distortion_plugin.h"
void distortion_plugin::initialization(const captcha_api &api) {

}
void distortion_plugin::release() {

}
config_define distortion_plugin::get_config_define() const {
  return nullptr;
}
void distortion_plugin::set_config(const captcha_config::config &node) {

}
captcha distortion_plugin::pipe(captcha &in) {
  cv::Mat image = static_cast<cv::Mat>(in);

  return captcha(image);
}

processor_plugin_interface *create() {
  return new distortion_plugin();
}

void destroy(processor_plugin_interface *plugin_interface) {
  delete plugin_interface;
}