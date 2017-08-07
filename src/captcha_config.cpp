//
// Created by zhsyourai on 8/7/17.
//

#include "captcha_config.h"

#include <utility>
#include <typeinfo>
using namespace captcha_config;

config_define::config_define() {

}

void config_define::add(std::string key, const std::string &regex) {
  config_define define;
  define.type = RULE_REGEX;
  define._rule.regex = regex;
  _defines[key] = define;
}

void config_define::add(std::string key, const std::vector<std::string> &list) {
  config_define define;
  define.type = RULE_ENUM;
  define._rule.enumerate = list;
  _defines[key] = define;
}

void config_define::add(std::string key, int64_t min, int64_t max) {
  config_define define;
  define.type = RULE_MIN_MAX;
  define._rule.min = min;
  define._rule.max = max;
  _defines[key] = define;
}
void config_define::add(std::string key, const config_define &value) {
  _defines[key] = value;
}
void config_define::add(std::string key, config_define &&value) {
  _defines[key] = value;
}

config_define &config_define::operator[](std::string key) {
  return _defines[key];
}
std::tuple<int64_t, int64_t> config_define::min_max() {
  return std::make_tuple(_rule.min, _rule.max);
}
std::string config_define::regex() {
  return _rule.regex;
}

node::node() : _type(TYPE_NONE) {
}

node &node::operator=(node &&other) noexcept {
  if (this == &other) {
    return *this;
  }
  _type = other._type;
  _key = std::move(other._key);
  switch (_type) {
    case TYPE_INT8:_i8_v = other._i8_v;
      break;
    case TYPE_INT16:_i16_v = other._i16_v;
      break;
    case TYPE_INT32:_i32_v = other._i32_v;
      break;
    case TYPE_STRING:_s_v = std::move(other._s_v);
      break;
    case TYPE_NONE:break;
  }
  return *this;
}

void node::add(std::string key, const std::string &value) {
  _key = std::move(key);
  _s_v = value;
  _type = TYPE_STRING;
}

void node::add(std::string key, std::string &&value) {
  _key = std::move(key);
  _s_v = value;
  _type = TYPE_STRING;
}

void node::add(std::string key, int8_t value) {
  _key = std::move(key);
  _i8_v = value;
  _type = TYPE_INT8;
}

void node::add(std::string key, int16_t value) {
  _key = std::move(key);
  _i16_v = value;
  _type = TYPE_INT16;
}

void node::add(std::string key, int32_t value) {
  _key = std::move(key);
  _i32_v = value;
  _type = TYPE_INT32;
}

void node::add(std::string key, const node &value) {
  _key = std::move(key);
  _type = TYPE_NODE;
  _node[key] = value;
}
void node::add(std::string key, node &&value) {
  _key = std::move(key);
  _type = TYPE_NODE;
  _node[key] = value;
}


std::string node::key() {
  return _key;
}

const node &node::value() {
  return *this;
}

node::operator int8_t() {
  if (_type != TYPE_INT8)
    throw std::bad_cast();
  return _i8_v;
}

node::operator int16_t() {
  if (_type != TYPE_INT8)
    throw std::bad_cast();
  return _i16_v;
}

node::operator int32_t() {
  if (_type != TYPE_INT8)
    throw std::bad_cast();
  return _i32_v;
}

node::operator std::string() {
  if (_type != TYPE_INT8)
    throw std::bad_cast();
  return _s_v;
}

