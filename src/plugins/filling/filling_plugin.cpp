//
// Created by zhsyourai on 8/4/17.
//

#include "filling_plugin.h"

void filling_plugin::initialization(const captcha_api &api) {

}

void filling_plugin::release() {

}

const captcha_config::config_define &filling_plugin::get_config_define() const {

}

void filling_plugin::set_config(const captcha_config::config &node) {

}

captcha &filling_plugin::pipe(captcha &in) {

}

processor_plugin_interface *create() {
  return new filling_plugin();
}

void destroy(processor_plugin_interface *plugin_interface) {
  delete plugin_interface;
}
