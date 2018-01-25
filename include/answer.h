//
// Created by zhsyourai on 8/16/17.
//

#ifndef XCAPTCHA_ANSWER_H
#define XCAPTCHA_ANSWER_H

#include <cstdint>
#include <string>
#include <vector>
#include "json.hpp"

// for convenience
using json = nlohmann::json;
enum class answer_type : char {
  ANSWER_CODE,
  ANSWER_RANGE,
  ANSWER_ORDER_RANGE,
  ANSWER_NONE,
};

namespace as {
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

void to_json(json &j, const order_range &o);

}

class answer {

 public:
  answer();
  explicit answer(answer_type _type);

  void set_type(answer_type type);
  void set_code(const std::string &code);
  void set_ranges(const std::vector<as::order_range> &ranges);

  answer_type get_type() const;
  std::string get_code() const;
  std::vector<as::order_range> get_ranges() const;

  std::string to_json() const;

  void clear();
 private:
  answer_type _type;
  std::string _code;
  std::vector<as::order_range> _ranges;
};

#endif //XCAPTCHA_ANSWER_H
