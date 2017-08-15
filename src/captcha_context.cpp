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
    for (auto pipe : pipes) {
      std::string plugin_name = pipe.first.as<std::string>();
      YAML::Node plugin_config = pipe.second;

      boost::filesystem::path file(plugin_path);
      file /= pipe.first.as<std::string>();
      file /= pipe.first.as<std::string>();
      file += "_plugin.so";

      plugins[plugin_name] = captcha_plugin_stub(file.string());
      plugins[plugin_name].get_interface()->initialization(api);
      const captcha_config::config_define &cd = plugins[plugin_name].get_interface()->get_config_define();
      captcha_config::config &&really = check_config(cd, plugin_config);
      plugins[plugin_name].get_interface()->set_config(really);
    }
  }
  return false;
}


void captcha_context::check_config(const captcha_config::config_define &config_define,
                                   const YAML::Node &plugin_config,
                                   captcha_config::config &config) {
  for (auto it : config_define) {
    if(it.second.is_container()) {
      captcha_config::config *inside = new captcha_config::config();
      check_config(it.second, plugin_config[it.first], *inside);
      config.insert(it.first, inside);
    } else if (plugin_config[it.first]) {
      config.insert(it.first, new captcha_config::config(plugin_config[it.first]));
    } else {
      config.insert(it.first, new captcha_config::config());
    }
  }
}

captcha_config::config captcha_context::check_config(const captcha_config::config_define &config_define,
                                    const YAML::Node &plugin_config) {
  captcha_config::config really(captcha_config::config::config_node_type::CONTAINER);

  for (auto it2 : config_define) {
    for (auto it3 : it2.second) {
      std::cout << it3.first << " -----> " << it3.second.as<uint32_t>() << std::endl;
      really.insert(it3.first, new captcha_config::config(it3.second.as<uint32_t>()));
    }
  }
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
