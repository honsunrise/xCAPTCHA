//
// Created by zhsyourai on 8/4/17.
//

#ifndef XCAPTCHA_CAPTCHA_CONTEXT_H
#define XCAPTCHA_CAPTCHA_CONTEXT_H
#include <string>
#include <yaml-cpp/yaml.h>
#include "captcha.h"
#include "captcha_plugin_stub.h"

class captcha_context {
 public:
  captcha_context();
  bool load_config(const std::string &path);
  captcha_config::config check_config(const captcha_config::config_define &config_define,
                                      const YAML::Node &plugin_config);
  captcha generate();
  virtual ~captcha_context();
 private:
  void check_config(const captcha_config::config_define &config_define,
                    const YAML::Node &plugin_config, captcha_config::config &config);

  captcha_api api;
  std::string config_path;
  int32_t width;
  int32_t height;
  int32_t bits;
  std::string plugin_path;
  std::map<std::string, captcha_plugin_stub> plugins;
  void check_config(const config_define &config_define, const YAML::Node &plugin_config, config &cfg);
};

#endif //XCAPTCHA_CAPTCHA_CONTEXT_H
