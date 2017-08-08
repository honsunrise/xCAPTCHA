//
// Created by zhsyourai on 8/4/17.
//

#include <yaml-cpp/yaml.h>
#include "captcha_context.h"

captcha_context::captcha_context() {

}
bool captcha_context::load_config(const std::string &path) {
  config_path = path;
  YAML::Node config = YAML::LoadFile(config_path);
  if(config["global"]) {
    std::cout << "Global" << std::endl;
  }
  if(config["pipes"]) {
    std::for_each(config["pipes"].begin(), config["pipes"].end(), [](const YAML::detail::iterator_value& n){
      std::cout << n.first << std::endl;
    });
  }
  return false;
}

captcha captcha_context::generate() {
  return captcha();
}
