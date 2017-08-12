//
// Created by zhsyourai on 8/4/17.
//

#ifndef XCAPTCHA_CAPTCHA_CONTEXT_H
#define XCAPTCHA_CAPTCHA_CONTEXT_H
#include <string>
#include "captcha.h"
#include "../src/captcha_plugin_stub.h"

class captcha_context {
 public:
  captcha_context();
  bool load_config(const std::string &path);
  captcha generate();
  virtual ~captcha_context();
 private:
  captcha_api api;
  std::string config_path;
  int32_t width;
  int32_t height;
  int32_t bits;
  std::string plugin_path;
  std::map<std::string, captcha_plugin_stub> plugins;
};

#endif //XCAPTCHA_CAPTCHA_CONTEXT_H
