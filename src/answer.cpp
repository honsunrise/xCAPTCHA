//
// Created by zhsyourai on 8/16/17.
//

#include "answer.h"
order_range::order_range() : x(0), y(0), width(0), height(0), order(-1) {}
order_range::order_range(int32_t x, int32_t y) : x(x), y(y), width(0), height(0), order(-1) {}
order_range::order_range(int32_t x, int32_t y, int32_t order) : x(x), y(y), width(0), height(0), order(order) {}
order_range::order_range(int32_t x, int32_t y, int32_t width, int32_t height)
    : x(x), y(y), width(width), height(height), order(-1) {}
order_range::order_range(int32_t x, int32_t y, int32_t width, int32_t height, int32_t order)
    : x(x), y(y), width(width), height(height), order(order) {}


answer::answer():_type(answer_type::ANSWER_NONE) {}

answer::answer(answer_type _type) : _type(_type) {}
void answer::set_type() {

}
void answer::set_code() {

}
void answer::set_ranges() {

}
answer_type answer::get_type() {
  return answer_type::ANSWER_RANGE;
}
std::string answer::get_code() {
  return std::__cxx11::string();
}
std::vector<order_range> answer::get_ranges() {
  return std::vector<order_range>();
}
void answer::clear() {

}
