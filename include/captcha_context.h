//
// Created by zhsyourai on 8/4/17.
//

#ifndef XCAPTCHA_CAPTCHA_CONTEXT_H
#define XCAPTCHA_CAPTCHA_CONTEXT_H
#include <string>
#include <yaml-cpp/yaml.h>
#include <boost/noncopyable.hpp>
#include "captcha.h"
#include "captcha_plugin_stub.h"
using namespace captcha_config;

struct order_plugin {
  int order;
  std::string plugin_name;
};

class captcha_context : boost::noncopyable {
 public:
  captcha_context() = default;
  bool load_config(const std::string &path);
  config check_config(const config_define &config_define,
                      const YAML::Node &plugin_config);
  captcha generate();
 private:
  void check_config(const config_define &config_define,
                    const YAML::Node &plugin_config, config &cfg);

  captcha_api api;
  std::string config_path;
  int32_t width;
  int32_t height;
  int32_t bits;
  std::string plugin_path;
  std::vector<order_plugin> plugins_order;
  std::map<std::string, captcha_plugin_stub> plugins;
};

#endif //XCAPTCHA_CAPTCHA_CONTEXT_H
