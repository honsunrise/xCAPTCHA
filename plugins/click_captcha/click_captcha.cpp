//
// Created by zhsyourai on 8/27/17.
//

#include <opencv2/freetype.hpp>
#include "click_captcha.h"
void click_captcha::initialization(const captcha_api &api) {
  this->api = &api;
}

void click_captcha::release() {

}

config_define click_captcha::get_config_define() const {
  return nullptr;
}

void click_captcha::set_config(const captcha_config::config &node) {

}

float computRectJoinUnion(const cv::Rect &rc1, const cv::Rect &rc2) {
  float AJoin, AUnion;
  CvPoint p1, p2;
  p1.x = std::max(rc1.x, rc2.x);
  p1.y = std::max(rc1.y, rc2.y);

  p2.x = std::min(rc1.x + rc1.width, rc2.x + rc2.width);
  p2.y = std::min(rc1.y + rc1.height, rc2.y + rc2.height);

  AJoin = 0;
  if (p2.x > p1.x && p2.y > p1.y) {
    AJoin = (p2.x - p1.x) * (p2.y - p1.y);
  }
  float A1 = rc1.width * rc1.height;
  float A2 = rc2.width * rc2.height;
  AUnion = (A1 + A2 - AJoin);

  if (AUnion > 0)
    return (AJoin / AUnion);
  return 0;
}

captcha click_captcha::pipe(captcha &in) {
  static std::random_device rd;
  static auto
      dice =
      std::bind(std::uniform_int_distribution<int32_t>(random_min, random_max), std::default_random_engine(rd()));
  static cv::Ptr<cv::freetype::FreeType2> ft2 = cv::freetype::createFreeType2();
  ft2->loadFontData("./resource/fonts/font1.ttf", 0);
  cv::Mat image = in;
  std::vector<cv::Rect> other_char(char_num);
  for (int i = 0; i < char_num; ++i) {
    int baseline = 0;
    std::string text;
    uint32_t code = api->random_char(std::locale("zh_CN.UTF8"));
    if ((code & 0xFF000000)) text += (code & 0xFF000000) >> 24;
    if ((code & 0xFF0000)) text += (code & 0xFF0000) >> 16;
    if ((code & 0xFF00)) text += (code & 0xFF00) >> 8;
    if ((code & 0xFF)) text += (code & 0xFF);

    cv::Size textSize = ft2->getTextSize(text, font_height, thickness, &baseline);
    if (thickness > 0) {
      baseline += thickness;
    }
    auto o_x = std::bind(std::uniform_int_distribution<int32_t>(textSize.width, image.cols - textSize.width),
                         std::default_random_engine(rd()));
    auto o_y = std::bind(std::uniform_int_distribution<int32_t>(textSize.height, image.rows - textSize.height),
                         std::default_random_engine(rd()));

    cv::Point text_org(o_x(), o_y());
    cv::Rect box(text_org + cv::Point(0, baseline), text_org + cv::Point(textSize.width, -textSize.height));
    if (!other_char.empty()) {
      for (auto it = other_char.begin(); it != other_char.end();) {
        float present = computRectJoinUnion(box, *it);
        if (present > join_persent) {
          text_org = cv::Point(o_x(), o_y());
          box = cv::Rect(text_org + cv::Point(0, baseline), text_org + cv::Point(textSize.width, -textSize.height));
          it = other_char.begin();
        } else {
          it++;
        }
      }
    }
    other_char.push_back(box);

    if (is_draw_box)
      rectangle(image, box, cv::Scalar(0, 255, 0), 1, 8);

    if (is_draw_base_line)
      line(image, text_org + cv::Point(0, thickness),
           text_org + cv::Point(textSize.width, thickness),
           cv::Scalar(0, 0, 255), 1, 8);

    int32_t t_r = r > 0 ? r : dice();
    int32_t t_g = g > 0 ? g : dice();
    int32_t t_b = b > 0 ? b : dice();
    const cv::Scalar &color = cv::Scalar(t_b, t_g, t_r);

    ft2->putText(image, text, text_org, font_height,
                 color, thickness, CV_AA, true);

  }
  return captcha(image);
}

processor_plugin_interface *create() {
  return new click_captcha();
}

void destroy(processor_plugin_interface *plugin_interface) {
  delete plugin_interface;
}
