//
// Created by zhsyourai on 8/4/17.
//

#ifndef XCAPTCHA_PLUGIN_INTERFACE_H
#define XCAPTCHA_PLUGIN_INTERFACE_H
#include "captcha.h"
#include "captcha_context.h"

class processor_plugin_interface {
 public:
  virtual void initialization(const captcha_context *context) = 0;
  virtual void release(const captcha_context *context) = 0;
  virtual captcha &pipe(captcha &in) = 0;
};

inline captcha& operator>>(captcha& in, processor_plugin_interface& plugin) {
  return plugin.pipe(in);
}
#endif //XCAPTCHA_PLUGIN_INTERFACE_H
