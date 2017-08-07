//
// Created by zhsyourai on 8/4/17.
//

#ifndef XCAPTCHA_CAPTCHA_CONTEXT_H
#define XCAPTCHA_CAPTCHA_CONTEXT_H
#include <string>
#include "captcha.h"

class captcha_context {
 public:
  captcha_context();
  bool load_config(std::string path);
  captcha generate();
 private:
  std::string config_path;
  int32_t width;
  int32_t height;
  int32_t bit;
};

#endif //XCAPTCHA_CAPTCHA_CONTEXT_H
