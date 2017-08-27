//
// Created by zhsyourai on 8/4/17.
//
#include <boost/filesystem.hpp>
#include "background_plugin.h"

void background_plugin::initialization(const captcha_api &api) {
  this->api = &api;
}

void background_plugin::release() {

}

captcha_config::config_define background_plugin::get_config_define() const {
  captcha_config::config_define define;
  define["image_dir"] = "";
  return define;
}

void background_plugin::set_config(const captcha_config::config &node) {
  using namespace boost::filesystem;
  image_dir = node["image_dir"];
  if (is_directory(image_dir)) {
    // TODO throw exception
  }
  file_count = std::count_if(
      directory_iterator(image_dir),
      directory_iterator(),
      static_cast<bool (*)(const path &)>(is_regular_file));
  if (is_directory(image_dir)) {
    int i = 0;
    for (directory_iterator it(image_dir); it != directory_iterator(); ++it) {
      if (is_regular(*it)) {
        file_map[i++] = it->path().filename().string();
      }
    }
  }
}

captcha background_plugin::pipe(captcha &in) {
  using namespace boost::filesystem;
  static std::random_device rd;
  static auto engine = std::default_random_engine(rd());
  static auto dice = std::bind(std::uniform_int_distribution<int32_t>(0, file_count-1), engine);
  cv::Mat image = in;
  path image_file = image_dir;
  image_file /= file_map[dice()];
  cv::Mat load = cv::imread(image_file.string());
  std::uniform_int_distribution<int32_t> range_row(0, std::max(load.rows - image.rows, 0));
  std::uniform_int_distribution<int32_t> range_col(0, std::max(load.cols - image.cols, 0));
  if(image.cols > load.cols || image.rows > load.rows) {
    int h = std::max(image.rows, load.rows);
    int w = std::max(image.cols, load.cols);
    cv::resize(load, load, cv::Size(w, h));
  }
  cv::Rect roi(range_col(engine), range_row(engine),image.cols, image.rows);
  load = load(roi);
  cv::Mat dst;
  cv::addWeighted(load, 0.8, image, 0.2, 0.0, dst);
  return captcha(load);
}

processor_plugin_interface *create() {
  return new background_plugin();
}

void destroy(processor_plugin_interface *plugin_interface) {
  delete plugin_interface;
}
