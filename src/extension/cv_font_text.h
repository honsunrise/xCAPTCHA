//
// Created by zhsyourai on 8/26/17.
//

#ifndef XCAPTCHA_CV_FONT_TEXT_H
#define XCAPTCHA_CV_FONT_TEXT_H

#include <boost/noncopyable.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/freetype.hpp>
using namespace cv;
class cv_font_text : boost::noncopyable {

  cv_font_text(const char *font_file);
  virtual ~cv_font_text();

  void putText(
      InputOutputArray img, const String &text, Point org,
      int fontHeight, Scalar color,
      int thickness, int line_type, bool bottomLeftOrigin
  );

  Size getTextSize(const String& text, int fontHeight, int thickness, int* baseLine);
 private:
  void putWChar(cv::InputOutputArray img, wchar_t wc, cv::Point &pos, cv::Scalar color);
 private:
  int m_fontType;
  cv::Scalar m_fontSize;
  bool m_fontUnderline;
};

#endif //XCAPTCHA_CV_FONT_TEXT_H
