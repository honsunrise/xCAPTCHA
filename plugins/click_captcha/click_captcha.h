//
// Created by zhsyourai on 8/27/17.
//

#ifndef XCAPTCHA_CLICK_CAPTCHA_H
#define XCAPTCHA_CLICK_CAPTCHA_H
#include "processor_plugin_interface.h"

class click_captcha : public processor_plugin_interface {
 public:
  void initialization(const captcha_api &api) override;
  void release() override;
  config_define get_config_define() const override;
  void set_config(const captcha_config::config &node) override;
  captcha pipe(captcha &in) override;
 private:
  const captcha_api *api;
  int32_t r = 0;
  int32_t g = 0;
  int32_t b = 0;
  uint32_t random_min = 0;
  uint32_t random_max = 255;
  bool is_draw_box = false;
  bool is_draw_base_line = false;
  int font_height = 40;
  int thickness = -1;
  uint32_t char_num = 4;
  float join_percent = 0.1;
  float alpha = 0.5;
  int32_t min_rotation = -180;
  int32_t max_rotation = 180;
  float min_scale = 0.5;
};

extern "C" {
processor_plugin_interface *create();

void destroy(processor_plugin_interface *plugin_interface);
}
#endif //XCAPTCHA_CLICK_CAPTCHA_H
