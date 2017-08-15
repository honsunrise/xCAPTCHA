//
// Created by zhsyourai on 8/4/17.
//

#ifndef XCAPTCHA_FILLING_PLUGIN_H
#define XCAPTCHA_FILLING_PLUGIN_H

#include <cstdint>
#include <cstddef>
#include "processor_plugin_interface.h"
class filling_plugin : public processor_plugin_interface {
 public:
  void initialization(const captcha_api &api) override;
  void release() override;
  captcha_config::config_define get_config_define() const override;
  void set_config(const captcha_config::config &node) override;
  captcha pipe(captcha &in) override;
 private:
  uint32_t r;
  uint32_t g;
  uint32_t b;
};

extern "C" {
processor_plugin_interface *create();

void destroy(processor_plugin_interface *plugin_interface);
}

#endif //XCAPTCHA_FILLING_PLUGIN_H
