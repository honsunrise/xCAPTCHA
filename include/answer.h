//
// Created by zhsyourai on 8/16/17.
//

#ifndef XCAPTCHA_ANSWER_H
#define XCAPTCHA_ANSWER_H

#include <cstdint>
#include <string>
#include <vector>

struct order_range {
  order_range();

  order_range(int32_t x, int32_t y);

  order_range(int32_t x, int32_t y, int32_t order);

  order_range(int32_t x, int32_t y, int32_t width, int32_t height);

  order_range(int32_t x, int32_t y, int32_t width, int32_t height, int32_t order);

  int32_t x;
  int32_t y;
  int32_t width;
  int32_t height;
  int32_t order;
};

enum class answer_type:char {
  ANSWER_CODE,
  ANSWER_RANGE,
  ANSWER_ORDER_RANGE,
  ANSWER_NONE,
};

class answer {
 public:
  answer();
  explicit answer(answer_type _type);

  void set_type();
  void set_code();
  void set_ranges();

  answer_type get_type();
  std::string get_code();
  std::vector<order_range> get_ranges();

  void clear();
 private:
  answer_type _type;
  std::string _code;
  std::vector<order_range> _ranges;
};

#endif //XCAPTCHA_ANSWER_H
