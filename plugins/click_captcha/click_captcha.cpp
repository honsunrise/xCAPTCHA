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

void click_captcha::pipe(captcha &in) const {
  static std::random_device rd;
  static auto
      dice =
      std::bind(std::uniform_int_distribution<int32_t>(random_min, random_max), std::default_random_engine(rd()));
  static auto
      char_number =
      std::bind(std::uniform_int_distribution<uint32_t>(min_char_num, max_char_num), std::default_random_engine(rd()));
  static auto
      select_number =
      std::bind(std::uniform_int_distribution<uint32_t>(min_select_num, max_select_num),
                std::default_random_engine(rd()));
  static cv::Ptr<cv::freetype::FreeType2> ft2 = cv::freetype::createFreeType2();
  ft2->loadFontData("./resource/fonts/font1.ttf", 0);

  static cv::Ptr<cv::freetype::FreeType2> ft2_q = cv::freetype::createFreeType2();
  ft2_q->loadFontData("./resource/fonts/black.ttf", 0);

  answer ans;
  image &img = in.get_image();
  std::vector<as::order_range> ranges;
  cv::Mat mat = img;
  uint32_t char_num = char_number();
  uint32_t select_num = select_number();
  std::vector<cv::Rect> other_char(char_num);
  std::string q;
  if (select_num == 1) {
    q = "点击图中 “&” 字";
  } else {
    if (is_order) {
      q = "按顺序点击图中 “&” 字";
    } else {
      q = "点击图中 “&” 字";
    }
  }
  std::string gen;
  for (int i = 0; i < char_num; ++i) {
    int baseline = 0;

    std::string text;
    uint32_t code = api->random_char(std::locale("zh_CN.UTF8"));
    if ((code & 0xFF000000)) text += (code & 0xFF000000) >> 24;
    if ((code & 0xFF0000)) text += (code & 0xFF0000) >> 16;
    if ((code & 0xFF00)) text += (code & 0xFF00) >> 8;
    if ((code & 0xFF)) text += (code & 0xFF);

    int q_height = 21;
    int q_box_height = q_height + 1;

    if (i == select_num - 1) {
      gen += text;
      q.replace(q.find('&'), 1, gen);
      cv::Size text_size = ft2_q->getTextSize(q, q_height, -1, &baseline);
      cv::Point text_org(0, q_height - baseline);
      cv::Rect box(cv::Point(0, 0), cv::Point(mat.cols, q_box_height));
      rectangle(mat, box, cv::Scalar::all(255), -1, CV_AA);
      ft2_q->putText(mat, q, text_org, q_height, cv::Scalar::all(0), thickness, CV_AA, true);
    } else {
      gen += text + "、";
    }

    baseline = 0;
    cv::Size text_size = ft2->getTextSize(text, font_height, thickness, &baseline);
    if (thickness > 0) {
      baseline += thickness;
    }
    auto o_x = std::bind(std::uniform_int_distribution<int32_t>(0, mat.cols - text_size.width),
                         std::default_random_engine(rd()));
    auto o_y = std::bind(std::uniform_int_distribution<int32_t>(q_box_height, mat.rows - text_size.height - baseline),
                         std::default_random_engine(rd()));

    cv::Point text_rand(o_x(), o_y());
    cv::Rect box_rand(text_rand, text_rand + cv::Point(text_size.width, text_size.height + baseline));
    if (!other_char.empty()) {
      for (auto it = other_char.begin(); it != other_char.end();) {
        float present = computRectJoinUnion(box_rand, *it);
        if (present > join_percent) {
          text_rand = cv::Point(o_x(), o_y());
          box_rand = cv::Rect(text_rand, text_rand + cv::Point(text_size.width, text_size.height + baseline));
          it = other_char.begin();
        } else {
          it++;
        }
      }
    }
    other_char.push_back(box_rand);
    ranges.emplace_back(box_rand.x, box_rand.y, box_rand.width, box_rand.height, i);
    // TODO:
    ans.set_type();
    ans.set_ranges();
    {
      cv::Mat text_image(text_size.height + baseline, text_size.width, mat.type(), cv::Scalar::all(255));
      cv::Mat mask(text_size.height + baseline, text_size.width, mat.type(), cv::Scalar::all(0));
      cv::Mat
          background = mat(cv::Rect(text_rand.x, text_rand.y, text_size.width, text_size.height + baseline)).clone();
      cv::Mat
          alpha_image = mat(cv::Rect(text_rand.x, text_rand.y, text_size.width, text_size.height + baseline)).clone();
      cv::Rect box(0, 0, text_size.width, text_size.height + baseline);
      if (is_draw_box)
        rectangle(text_image, box, cv::Scalar(0, 255, 0), 1, 8);

      if (is_draw_base_line)
        line(text_image,
             cv::Point(0, text_size.height),
             cv::Point(text_size.width, text_size.height),
             cv::Scalar(0, 0, 255),
             1,
             8);

      int32_t t_r = r > 0 ? r : dice();
      int32_t t_g = g > 0 ? g : dice();
      int32_t t_b = b > 0 ? b : dice();
      const cv::Scalar &color = cv::Scalar(t_b, t_g, t_r);

      ft2->putText(text_image, text, cv::Point(0, text_size.height), font_height, color, thickness, CV_AA, true);
      ft2->putText(mask,
                   text,
                   cv::Point(0, text_size.height),
                   font_height,
                   cv::Scalar::all(255),
                   thickness,
                   CV_AA,
                   true);

      Mat warp_mat(2, 3, CV_32FC1);
      cv::Point center = cv::Point(text_image.cols / 2, text_image.rows / 2);
      auto angle = std::bind(std::uniform_int_distribution<int32_t>(min_rotation, max_rotation),
                             std::default_random_engine(rd()));
      auto scale = std::bind(std::uniform_real_distribution<float>(min_scale, 1.0), std::default_random_engine(rd()));
      warp_mat = cv::getRotationMatrix2D(center, angle(), scale());
      warpAffine(text_image, text_image, warp_mat, text_image.size());
      warpAffine(mask, mask, warp_mat, mask.size());
      mask.convertTo(mask, CV_32FC3, 1.0 / 255);
      text_image.convertTo(text_image, CV_32FC3);
      background.convertTo(background, CV_32FC3);
      alpha_image.convertTo(alpha_image, CV_32FC3);
      cv::multiply(mask, text_image, text_image);
      cv::multiply(mask, alpha_image, alpha_image);
      cv::addWeighted(text_image, alpha, alpha_image, 1 - alpha, 0, text_image);
      cv::multiply(cv::Scalar::all(1.0) - mask, background, background);
      cv::add(text_image, background, text_image);
      text_image.convertTo(text_image, mat.type());
      text_image.copyTo(mat(cv::Rect(text_rand.x, text_rand.y, text_size.width, text_size.height + baseline)));
    }

  }
  in = captcha(image(mat), std::move(ans));
}

processor_plugin_interface *create() {
  return new click_captcha();
}

void destroy(processor_plugin_interface *plugin_interface) {
  delete plugin_interface;
}
