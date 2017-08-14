//
// Created by zhsyourai on 8/4/17.
//

#ifndef XCAPTCHA_FILLING_PLUGIN_H
#define XCAPTCHA_FILLING_PLUGIN_H

#include "processor_plugin_interface.h"
class filling_plugin : public processor_plugin_interface {
  void initialization(const captcha_api &api) override;
  void release() override;
  const captcha_config::config_define &get_config_define() const override;
  void set_config(const captcha_config::config &node) override;
  captcha &pipe(captcha &in) override;
};

extern "C" {
processor_plugin_interface *create();

void destroy(processor_plugin_interface *plugin_interface);
}

#endif //XCAPTCHA_FILLING_PLUGIN_H
