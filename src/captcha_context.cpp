//
// Created by zhsyourai on 8/4/17.
//

#include <yaml-cpp/yaml.h>
#include <boost/filesystem.hpp>
#include "captcha_context.h"

captcha_context::captcha_context() {

}


captcha_context::~captcha_context() {

}

bool captcha_context::load_config(const std::string &path) {
  config_path = path;
  YAML::Node config = YAML::LoadFile(config_path);
  auto &&global = config["global"];
  if (global) {
    if (global["width"]) {
      width = global["width"].as<int32_t>();
    } else {
      width = 128;
    }
    if (global["height"]) {
      height = global["height"].as<int32_t>();
    } else {
      height = 64;
    }
    if (global["bits"]) {
      bits = global["bits"].as<int32_t>();
    } else {
      bits = 32;
    }
    if (global["plugin"]) {
      if (global["plugin"]["path"]) {
        plugin_path = global["plugin"]["path"].as<std::string>();
      } else {
        plugin_path = ".";
      }
    } else {
      plugin_path = ".";
    }

  }
  auto &&pipes = config["pipes"];
  if (pipes) {
    for (auto it = pipes.begin(); it != pipes.end(); ++it) {
      std::string plugin_name = it->first.as<std::string>();
      YAML::Node plugin_config = it->second;

      boost::filesystem::path file(plugin_path);
      file /= it->first.as<std::string>();
      file /= it->first.as<std::string>();
      file += "_plugin.so";
      plugins[plugin_name] = captcha_plugin_stub(file.string());
      plugins[plugin_name].get_interface()->initialization(api);
      const captcha_config::config_define &cd = plugins[plugin_name].get_interface()->get_config_define();
    }
  }
  return false;
}

captcha captcha_context::generate() {
  return captcha();
}
