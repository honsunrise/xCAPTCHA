//
// Created by zhsyourai on 8/4/17.
//

#include "noise_plugin.h"

void noise_plugin::initialization(const captcha_api &api) {
  this->api = &api;
}

void noise_plugin::release() {
  this->api = nullptr;
}

captcha_config::config_define noise_plugin::get_config_define() const {
  captcha_config::config_define define;
  define["color"] = captcha_config::config_define();
  define["color"]["r"] = -1;
  define["color"]["g"] = -1;
  define["color"]["b"] = -1;
  define["color"]["random"] = captcha_config::config_define();
  define["color"]["random"]["min"] = 0U;
  define["color"]["random"]["max"] = 256U;

  define["type"] = "line";
  define["param"] = captcha_config::config_define();
  define["param"]["line_number"] = 100;
  return define;
}

void noise_plugin::set_config(const captcha_config::config &node) {
  const auto &color = node["color"];
  r = color["r"];
  g = color["g"];
  b = color["b"];

  const auto &random = node["color"]["random"];
  random_min = random["min"];
  random_max = random["max"];

  type = node["type"];
  const auto &param = node["param"];
  line_number = param["line_number"];
}

int noise_plugin::drawing_random_lines(Mat image, int x_1, int y_1, int x_2, int y_2) const {
  static std::random_device rd;
  static auto
      c = std::bind(std::uniform_int_distribution<int32_t>(random_min, random_max), std::default_random_engine(rd()));
  static auto
      w = std::bind(std::uniform_int_distribution<int32_t>(x_1, x_2), std::default_random_engine(rd()));
  static auto
      h = std::bind(std::uniform_int_distribution<int32_t>(y_1, y_2), std::default_random_engine(rd()));

  Point pt1, pt2;
  for (int i = 0; i < line_number; i++) {
    pt1.x = w();
    pt1.y = h();
    pt2.x = w();
    pt2.y = h();

    int t_r = r > 0 ? r : c();
    int t_g = g > 0 ? g : c();
    int t_b = b > 0 ? b : c();

    line(image, pt1, pt2, Scalar(t_b & 255, t_g & 255, t_r & 255), 1, LINE_AA);
  }
  return 0;
}

void noise_plugin::pipe(captcha &in) const {
  image &img = in.get_image();
  Mat mat = img;
  drawing_random_lines(mat, 0, 0, mat.cols, mat.rows);
  img = image(mat);
}
int noise_plugin::drawing_random_wave(Mat image, int x_1, int y_1, int x_2, int y_2) const {
  return 0;
}
int noise_plugin::drawing_random_circles(Mat image, int x_1, int y_1, int x_2, int y_2) const {
  return 0;
}

processor_plugin_interface *create() {
  return new noise_plugin();
}

void destroy(processor_plugin_interface *plugin_interface) {
  delete plugin_interface;
}
