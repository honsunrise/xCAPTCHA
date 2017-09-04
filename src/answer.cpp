//
// Created by zhsyourai on 8/16/17.
//

#include "answer.h"

as::order_range::order_range() : x(0), y(0), width(0), height(0), order(-1) {}

as::order_range::order_range(int32_t x, int32_t y) : x(x), y(y), width(0), height(0), order(-1) {}

as::order_range::order_range(int32_t x, int32_t y, int32_t order) : x(x), y(y), width(0), height(0), order(order) {}

as::order_range::order_range(int32_t x, int32_t y, int32_t width, int32_t height)
    : x(x), y(y), width(width), height(height), order(-1) {}

as::order_range::order_range(int32_t x, int32_t y, int32_t width, int32_t height, int32_t order)
    : x(x), y(y), width(width), height(height), order(order) {}

void as::to_json(json &j, const as::order_range &o) {
  j = json{{"x", o.x}, {"y", o.y}, {"width", o.width}, {"height", o.height}, {"order", o.order}};
}

answer::answer() : _type(answer_type::ANSWER_NONE) {}

answer::answer(answer_type _type) : _type(_type) {}

void answer::set_type(answer_type type) {
  _type = type;
}

void answer::set_code(const std::string &code) {
  _code = code;
}

void answer::set_ranges(const std::vector<as::order_range> &ranges) {
  _ranges = ranges;
}

answer_type answer::get_type() const {
  return answer_type::ANSWER_RANGE;
}

std::string answer::get_code() const {
  return _code;
}

std::vector<as::order_range> answer::get_ranges() const {
  return _ranges;
}

void answer::clear() {

}

static std::string answer_type_to_string(answer_type type) {
  switch (type) {
    case answer_type::ANSWER_CODE: return "ANSWER_CODE";
    case answer_type::ANSWER_RANGE:return "ANSWER_RANGE";
    case answer_type::ANSWER_ORDER_RANGE:return "ANSWER_ORDER_RANGE";
    case answer_type::ANSWER_NONE:return "ANSWER_NONE";
  }
}

std::string answer::to_json() const {
  json j;
  j["type"] = answer_type_to_string(this->_type);
  j["code"] = this->_code;
  j["ranges"] = this->_ranges;
  return j.dump();
}
