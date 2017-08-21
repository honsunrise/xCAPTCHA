//
// Created by zhsyourai on 8/4/17.
//

#ifndef XCAPTCHA_NOISE_PLUGIN_H
#define XCAPTCHA_NOISE_PLUGIN_H

#include <cstdint>
#include <cstddef>
#include <functional>
#include <random>
#include "processor_plugin_interface.h"
using namespace cv;

class noise_plugin : public processor_plugin_interface {
 public:
  void initialization(const captcha_api &api) override;
  void release() override;
  captcha_config::config_define get_config_define() const override;
  void set_config(const captcha_config::config &node) override;
  captcha pipe(captcha &in) override;
 private:
  int drawing_random_lines(Mat image, int x_1, int y_1, int x_2, int y_2);
  int drawing_random_wave(Mat image, int x_1, int y_1, int x_2, int y_2);
  int drawing_random_circles(Mat image, int x_1, int y_1, int x_2, int y_2);

  const captcha_api *api;
  int32_t r = 0;
  int32_t g = 0;
  int32_t b = 0;
  uint32_t random_min = 0;
  uint32_t random_max = 0;
  // Line
  uint32_t line_number = 0;

  std::string type;
};

extern "C" {
processor_plugin_interface *create();

void destroy(processor_plugin_interface *plugin_interface);
}

#endif //XCAPTCHA_NOISE_PLUGIN_H
