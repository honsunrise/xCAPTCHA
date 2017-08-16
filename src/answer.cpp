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
