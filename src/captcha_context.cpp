//
// Created by zhsyourai on 8/4/17.
//

#include "captcha_context.h"

#include <utility>
captcha_context::captcha_context() {

}
bool captcha_context::load_config(std::string path) {
  config_path = std::move(path);

  return false;
}

captcha captcha_context::generate() {
  return captcha();
}
