//
// Created by zhsyourai on 8/4/17.
//
#include <boost/filesystem.hpp>
#include <random>
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

void background_plugin::pipe(captcha &in) const {
  using namespace boost::filesystem;
  static std::random_device rd;
  static auto engine = std::default_random_engine(rd());
  static auto dice = std::bind(std::uniform_int_distribution<uint32_t>(0, file_count - 1), engine);
  image &img = in.get_image();
  cv::Mat mat = img;
  path image_file = image_dir;
  image_file /= file_map.at(dice());
  cv::Mat load = cv::imread(image_file.string());
  std::uniform_int_distribution<int32_t> range_row(0, std::max(load.rows - mat.rows, 0));
  std::uniform_int_distribution<int32_t> range_col(0, std::max(load.cols - mat.cols, 0));
  if (mat.cols > load.cols || mat.rows > load.rows) {
    int h = std::max(mat.rows, load.rows);
    int w = std::max(mat.cols, load.cols);
    cv::resize(load, load, cv::Size(w, h));
  }
  cv::Rect roi(range_col(engine), range_row(engine), mat.cols, mat.rows);
  load = load(roi);
  cv::Mat dst;
  cv::addWeighted(load, 0.8, mat, 0.2, 0.0, dst);
  img = image(load);
}

processor_plugin_interface *create() {
  return new background_plugin();
}

void destroy(processor_plugin_interface *plugin_interface) {
  delete plugin_interface;
}
