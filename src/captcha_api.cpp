//
// Created by zhsyourai on 8/25/17.
//

#include <codecvt>
#include <random>
#include "captcha_api.h"
#include "deformation/image_deformation_mls.h"
#include "charset.h"

bool captcha_api::get_env(std::string key) const {
  return false;
}

Mat captcha_api::deformation(deformation_type type,
                             const vector<Point2d> &q,
                             const vector<Point2d> &p,
                             const double alpha,
                             size_t grid_size,
                             const Mat &src) const {
  image_deformation_engine *engine = nullptr;
  switch (type) {
    case AFFINE:engine = new id_engine_affine();
      break;
    case GRID:engine = new id_engine_grid();
      break;
    case RIGID:engine = new id_engine_rigid();
      break;
    case SIMILARITY:engine = new id_engine_similarity();
      break;
  }
  image_deformation_mls mls(engine, alpha, grid_size, src.cols, src.rows, q, p);
  return mls.genNewImg(src);
}

uint32_t captcha_api::random_char(std::locale locale) const {
  static std::random_device rd;
  static std::default_random_engine engine(rd());
  const wchar_t *charset = nullptr;
  std::uniform_int_distribution<int32_t> *distribution = nullptr;
  if (locale == std::locale("en_US.UTF8")) {
    distribution =
        new std::uniform_int_distribution<int32_t>(0, sizeof(english_charset) / sizeof(*english_charset) - 1);
    charset = english_charset;
  } else if (locale == std::locale("zh_CN.UTF8")) {
    distribution = new std::uniform_int_distribution<int32_t>(0,
                                                              sizeof(chinese_simplified_charset)
                                                                  / sizeof(*chinese_simplified_charset) - 1);
    charset = chinese_simplified_charset;
  } else if (locale == std::locale("zh_TW.UTF8")) {
    distribution = new std::uniform_int_distribution<int32_t>(0,
                                                              sizeof(chinese_traditional_charset)
                                                                  / sizeof(*chinese_traditional_charset) - 1);
    charset = chinese_traditional_charset;
  } else {
    distribution =
        new std::uniform_int_distribution<int32_t>(0, sizeof(english_charset) / sizeof(*english_charset) - 1);
    charset = english_charset;
  }
  int32_t pos = (*distribution)(engine);
  const wchar_t source = charset[pos];
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
  std::string dest = convert.to_bytes(source);
  delete distribution;
  uint32_t ret = 0;
  for (char a : dest) {
    ret <<= 8;
    ret |= static_cast<uint8_t >(a);
  }
  return ret;
}
