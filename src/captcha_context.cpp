//
// Created by zhsyourai on 8/4/17.
//

#include <boost/filesystem.hpp>
#include "captcha_context.h"
captcha_context::captcha_context() {

}

captcha_context::~captcha_context() {

}

bool captcha_context::load_config(const std::string &path) {
  config_path = path;
  YAML::Node raw_config = YAML::LoadFile(config_path);
  auto &&global = raw_config["global"];
  if (global.IsDefined()) {
    if (global["width"].IsDefined()) {
      width = global["width"].as<int32_t>();
    } else {
      width = 128;
    }
    if (global["height"].IsDefined()) {
      height = global["height"].as<int32_t>();
    } else {
      height = 64;
    }
    if (global["bits"].IsDefined()) {
      bits = global["bits"].as<int32_t>();
    } else {
      bits = 32;
    }
    if (global["plugin"].IsDefined()) {
      if (global["plugin"]["path"].IsDefined()) {
        plugin_path = global["plugin"]["path"].as<std::string>();
      } else {
        plugin_path = ".";
      }
    } else {
      plugin_path = ".";
    }

  }
  auto &&pipes = raw_config["pipes"];
  if (pipes.IsDefined()) {
    for (auto pipe : pipes) {
      std::string plugin_name = pipe.first.as<std::string>();
      YAML::Node plugin_config = pipe.second;
      order_plugin order;
      order.order = plugin_config["__order"].as<int>();
      order.plugin_name = plugin_name;
      plugins_order.push_back(order);
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
    std::sort(plugins_order.begin(), plugins_order.end(), [](order_plugin &a, order_plugin&b){
      return a.order < b.order;
    });
  }
  return false;
}

void captcha_context::check_config(const config_define &config_define, const YAML::Node &plugin_config, config &cfg) {
  for (auto &it : config_define::iterator_wrapper(config_define)) {
    if (it.value().is_map()) {
      config inside;
      check_config(it.value(), plugin_config[it.key()], inside);
      cfg[it.key()] = inside;
    } else if (plugin_config[it.key()].IsDefined()) {
      switch (it.value().type()) {
        case detail::value_t::string:cfg[it.key()] = plugin_config[it.key()].as<std::string>();
          break;
        case detail::value_t::boolean:cfg[it.key()] = plugin_config[it.key()].as<bool>();
          break;
        case detail::value_t::number_integer:cfg[it.key()] = plugin_config[it.key()].as<int64_t>();
          break;
        case detail::value_t::number_unsigned:cfg[it.key()] = plugin_config[it.key()].as<uint64_t>();
          break;
        case detail::value_t::number_float:cfg[it.key()] = plugin_config[it.key()].as<double>();
          break;
        case detail::value_t::discarded:
        case detail::value_t::null:
        case detail::value_t::map:
        case detail::value_t::array:break;
      }
    } else {
      cfg[it.key()] = config(config_define[it.key()]);
    }
  }
}

config captcha_context::check_config(const config_define &config_define,
                                     const YAML::Node &plugin_config) {
  config really;
  check_config(config_define, plugin_config, really);
  return really;
}

captcha captcha_context::generate() {
  cv::Mat image = cv::Mat::zeros(height, width, CV_8UC3);
  cv::rectangle(image,
                cv::Point(0, 0),
                cv::Point(width, height),
                cv::Scalar(0, 0, 0), -1, 8);
  captcha &&ca = captcha(image);
  for (const auto &plugin:plugins_order) {
    ca = plugins[plugin.plugin_name].get_interface()->pipe(ca);
  }
  return ca;
}

