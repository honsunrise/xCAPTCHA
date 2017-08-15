//
// Created by zhsyourai on 8/15/17.
//

#ifndef XCAPTCHA_RULES_H
#define XCAPTCHA_RULES_H
#include <string>
namespace captcha_config {
namespace rules {
struct _base_arg {
  _base_arg() {}
  virtual ~_base_arg() {}
  virtual _base_arg *clone() {};
};

struct string_arg : _base_arg {
  string_arg(const char *arg) : arg_value(arg) {}

  string_arg(const std::string &arg) : arg_value(arg) {}

  string_arg(std::string &&arg) : arg_value(std::move(arg)) {}

  _base_arg *clone() override {
    return new string_arg(arg_value);
  }

  std::string arg_value;
};

struct bool_arg : _base_arg {
  bool_arg(bool b) : arg_value(b) {}

  _base_arg *clone() override {
    return new bool_arg(arg_value);
  }

  bool arg_value;
};

template<typename T>
struct _integer_arg : _base_arg {
  _integer_arg(T b) {
    static_assert(std::is_integral<T>::value, "Integer required.");
    arg_value = b;
  }
  _base_arg *clone() override {
    return new _integer_arg(arg_value);
  }

  T arg_value;
};

template<typename T>
struct _list_arg : _base_arg {
  _list_arg(std::vector<T> &&b) : arg_value(std::move(b)) {}

  _list_arg(const std::vector<T> &b) : arg_value(b) {}

  _base_arg *clone() override {
    return new _list_arg(arg_value);
  }

  std::vector<T> arg_value;
};

struct int64_arg : _integer_arg<int64_t> {
  int64_arg(int64_t &&arg) : _integer_arg(std::forward<int64_t>(arg)) {}
};

struct string_list_arg : _list_arg<std::string> {
  string_list_arg(std::vector<std::string> &&arg) : _list_arg(std::forward<std::vector<std::string>>(arg)) {}
};

struct regex : string_arg {
  template<typename T>
  regex(T &&arg): string_arg(std::forward<T>(arg)) {}
};

struct maximum : int64_arg {
  maximum(int64_t &&arg) : int64_arg(std::forward<int64_t>(arg)) {}
};

struct minimum : int64_arg {
  minimum(int64_t &&arg) : int64_arg(std::forward<int64_t>(arg)) {}
};

struct enumerate : string_list_arg {
  enumerate(std::vector<std::string> &&arg) : string_list_arg(std::forward<std::vector<std::string>>(arg)) {}
};
}
}
#endif //XCAPTCHA_RULES_H
