//
// Created by zhsyourai on 8/4/17.
//

#ifndef XCAPTCHA_PLUGIN_INTERFACE_H
#define XCAPTCHA_PLUGIN_INTERFACE_H
#include "captcha.h"
#include "captcha_api.h"
#include "config/config.h"

using namespace captcha_config;
class processor_plugin_interface {
 public:
  virtual void initialization(const captcha_api &api) = 0;
  virtual void release() = 0;
  virtual captcha_config::config_define get_config_define() const = 0;
  virtual void set_config(const captcha_config::config &node) = 0;
  virtual void pipe(captcha &in) const = 0;
};

// the types of the class factories
typedef processor_plugin_interface* create_t();
typedef void destroy_t(processor_plugin_interface* plugin_interface);

inline captcha& operator>>(captcha& in, processor_plugin_interface& plugin) {
  plugin.pipe(in);
  return in;
}
#endif //XCAPTCHA_PLUGIN_INTERFACE_H
