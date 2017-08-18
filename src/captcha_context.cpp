//
// Created by zhsyourai on 8/4/17.
//

#include <boost/filesystem.hpp>
#include "captcha_context.h"
using namespace captcha_config;

captcha_context::captcha_context() {

}

captcha_context::~captcha_context() {

}

bool captcha_context::load_config(const std::string &path) {
  config_path = path;
  YAML::Node raw_config = YAML::LoadFile(config_path);
  auto &&global = raw_config["global"];
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
  auto &&pipes = raw_config["pipes"];
  if (pipes) {
    for (auto pipe : pipes) {
      std::string plugin_name = pipe.first.as<std::string>();
      YAML::Node plugin_config = pipe.second;

      boost::filesystem::path file(plugin_path);
      file /= pipe.first.as<std::string>();
      file /= pipe.first.as<std::string>();
      file += "_plugin.so";

      plugins[plugin_name] = captcha_plugin_stub(file.string());
      plugins[plugin_name].get_interface()->initialization(api);
      const config_define &cd = plugins[plugin_name].get_interface()->get_config_define();
      config &&really = check_config(cd, plugin_config);
      plugins[plugin_name].get_interface()->set_config(really);
    }
  }
  return false;
}

detail::placeholder* captcha_context::yaml_node_2_type(const YAML::Node &node, const std::type_info &type) {
  return new detail::content_holder<typename type>(node.as<typename type>());
}

void captcha_context::check_config(const config_define &config_define,
                                   const YAML::Node &plugin_config,
                                   config &cfg) {
  for (auto it : config_define) {
    if(it.second.is_container()) {
      config *inside = new config(config::config_node_type::CONTAINER);
      check_config(it.second, plugin_config[it.first], *inside);
      cfg.insert(it.first, inside);
    } else if (plugin_config[it.first]) {
      cfg.insert(it.first, new config(yaml_node_2_type(plugin_config[it.first], it.second.get_value()->type())));
    } else {
      cfg.insert(it.first, new config(it.second.get_value()));
    }
  }
}

config captcha_context::check_config(const config_define &config_define,
                                    const YAML::Node &plugin_config) {
  config really(config::config_node_type::CONTAINER);
  check_config(config_define, plugin_config, really);
  return really;
}

captcha captcha_context::generate() {
  cv::Mat image = cv::Mat::zeros(height, width, CV_8UC4);
  cv::rectangle(image,
                cv::Point(0, 0),
                cv::Point(width, height),
                cv::Scalar(0, 0, 0), -1, 8);
  captcha &&ca = captcha(image);
  for (auto plugin:plugins) {
    ca = plugin.second.get_interface()->pipe(ca);
  }
  return ca;
}
