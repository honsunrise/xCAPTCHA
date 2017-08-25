//
// Created by zhsyourai on 8/17/17.
//

#ifndef XCAPTCHA_DISTORTION_PLUGIN_H
#define XCAPTCHA_DISTORTION_PLUGIN_H

#include <cstdint>
#include <cstddef>
#include "processor_plugin_interface.h"
class deformation_plugin : public processor_plugin_interface {
 public:
  void initialization(const captcha_api &api) override;
  void release() override;
  config_define get_config_define() const override;
  void set_config(const captcha_config::config &node) override;
  captcha pipe(captcha &in) override;
 private:
  const captcha_api *api;
};

extern "C" {
processor_plugin_interface *create();

void destroy(processor_plugin_interface *plugin_interface);
}
#endif //XCAPTCHA_DISTORTION_PLUGIN_H
