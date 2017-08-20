//
// Created by zhsyourai on 8/15/17.
//

#ifndef XCAPTCHA_RULES_H
#define XCAPTCHA_RULES_H
#include <string>
#include <vector>
namespace captcha_config {
namespace rules {

struct _base_rule {
  _base_rule() = default;
  virtual ~_base_rule() = default;
  virtual _base_rule *clone() {};
};

struct string_rule : _base_rule {
  string_rule(const char *arg) : arg_value(arg) {}

  string_rule(const std::string &arg) : arg_value(arg) {}

  string_rule(std::string &&arg) : arg_value(std::move(arg)) {}

  _base_rule *clone() override {
    return new string_rule(arg_value);
  }

  std::string arg_value;
};

struct bool_rule : _base_rule {
  bool_rule(bool b) : arg_value(b) {}

  _base_rule *clone() override {
    return new bool_rule(arg_value);
  }

  bool arg_value;
};

template<typename T>
struct _integer_rule : _base_rule {
  _integer_rule(T b) {
    static_assert(std::is_integral<T>::value, "Integer required.");
    arg_value = b;
  }
  _base_rule *clone() override {
    return new _integer_rule(arg_value);
  }

  T arg_value;
};

template<typename T>
struct _list_rule : _base_rule {
  _list_rule(std::vector<T> &&b) : arg_value(std::move(b)) {}

  _list_rule(const std::vector<T> &b) : arg_value(b) {}

  _base_rule *clone() override {
    return new _list_rule(arg_value);
  }

  std::vector<T> arg_value;
};

struct int64_rule : _integer_rule<int64_t> {
  int64_rule(int64_t &&arg) : _integer_rule(std::forward<int64_t>(arg)) {}
};

struct string_list_rule : _list_rule<std::string> {
  string_list_rule(std::vector<std::string> &&arg) : _list_rule(std::forward<std::vector<std::string>>(arg)) {}
};

struct regex : string_rule {
  regex(const char *arg) : string_rule(arg) {}
  regex(const std::string &arg) : string_rule(arg) {}
  regex(std::string &&arg) : string_rule(arg) {}
};

struct maximum : int64_rule {
  maximum(int64_t &&arg) : int64_rule(std::forward<int64_t>(arg)) {}
};

struct minimum : int64_rule {
  minimum(int64_t &&arg) : int64_rule(std::forward<int64_t>(arg)) {}
};

struct enumerate : string_list_rule {
  enumerate(std::vector<std::string> &&arg) : string_list_rule(std::forward<std::vector<std::string>>(arg)) {}
};

}
}
#endif //XCAPTCHA_RULES_H
