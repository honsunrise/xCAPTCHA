//
// Created by zhsyourai on 8/7/17.
//

#ifndef XCAPTCHA_CAPTCHA_CONFIG_H
#define XCAPTCHA_CAPTCHA_CONFIG_H
#include <string>
#include <vector>
#include <map>
#include <typeinfo>
namespace captcha_config {

namespace utils {
template<typename... T>
struct param_pack {
};

template<typename F, typename T>
struct has_type;

template<typename F>
struct has_type<F, param_pack<>> {
  static constexpr bool value = false;
};

template<typename F, typename H, typename... T>
struct has_type<F, param_pack<H, T...>> {
  static constexpr bool value =
      std::is_same<F, typename std::decay<H>::type>::value ? true : has_type<F, param_pack<T...>>::value;
};
}

struct _base_arg {
  _base_arg() {}
  virtual ~_base_arg() {}
};

struct string_arg : _base_arg {
  string_arg(const char *arg) : arg_value(arg) {}

  string_arg(std::string &&arg) : arg_value(std::move(arg)) {}

  string_arg(std::string arg) : arg_value(std::move(arg)) {}

  std::string arg_value;
};

struct bool_arg : _base_arg {
  bool_arg(bool b) : arg_value(b) {}

  bool arg_value;
};

template<typename T>
struct _integer_arg : _base_arg {
  _integer_arg(T b) {
    static_assert(std::is_integral<T>::value, "Integer required.");
    arg_value = b;
  }

  T arg_value;
};

template<typename T>
struct _list_arg : _base_arg {
  _list_arg(std::vector<T> &&b) {
    arg_value = std::move(b);
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

class config_define {
  class config_define_helper {
   public:
    config_define_helper(config_define &cd) : cd(cd) {}

    void set(regex &&v) {
      cd._rule = new regex(v);
    }
    void set(minimum &&v) {
      cd._rule = new minimum(v);
    }
    void set(maximum &&v) {
      cd._rule = new maximum(v);
    }
    void set(enumerate &&v) {
      cd._rule = new enumerate(v);
    }
    void set(config_define &&v) {
      cd = v;
    }
   private:
    config_define &cd;
  };

 public:
  config_define() : _rule(nullptr) {}

  template<typename ValueType>
  config_define(const ValueType &value) {
    config_define_helper cdh(*this);
    cdh.set(value);
  }

  config_define(const config_define &other) : _rule(other._rule) {
  }

  config_define(config_define &&other) noexcept : _rule(other._rule) {
    other._rule = nullptr;
  }

  virtual ~config_define() {
    delete _rule;
  }

  config_define &operator=(const config_define &rhs) {
    _rule = rhs._rule;
    _defines = rhs._defines;
    return *this;
  }

  config_define &operator=(config_define &&rhs) noexcept {
    std::swap(_rule, rhs._rule);
    std::swap(_defines, rhs._defines);
    rhs._rule = nullptr;
    rhs._defines.clear();
    return *this;
  }

  template<typename Args>
  void add(const std::string &key, Args &&args) {
    config_define cd;
    config_define_helper cdh(cd);
    cdh.set(std::forward<Args>(args));
    _defines[key] = cd;
  }

  config_define &operator[](const std::string &key) {
    return _defines[key];
  }

  explicit operator regex() {
    return *dynamic_cast<regex *>(_rule);
  }

  explicit operator minimum() {
    return *dynamic_cast<minimum *>(_rule);
  }

  explicit operator maximum() {
    return *dynamic_cast<maximum *>(_rule);
  }

  explicit operator enumerate() {
    return *dynamic_cast<enumerate *>(_rule);
  }

 private:
  _base_arg *_rule;
  std::map<std::string, config_define> _defines;
};

class captcha_node {
  class node_helper {
   public:
    node_helper(captcha_node &n) : n(n) {}

    void set(int64_arg &&v) {
      n._content = new int64_arg(v);
    }
    void set(string_arg &&v) {
      n._content = new string_arg(v);
    }
    void set(bool_arg &&v) {
      n._content = new bool_arg(v);
    }
    void set(enumerate &&v) {
      n._content = new enumerate(v);
    }
    void set(captcha_node &&v) {
      n = v;
    }
   private:
    captcha_node &n;
  };

 public:
  captcha_node() : _content(nullptr) {}

  template<typename ValueType>
  captcha_node(const ValueType &value) {
    node_helper cdh(*this);
    cdh.set(value);
  }

  captcha_node(const captcha_node &other) : _content(other._content) {
  }

  captcha_node(captcha_node &&other) noexcept : _content(other._content) {
    other._content = nullptr;
  }

  virtual ~captcha_node() {
    delete _content;
  }

  captcha_node &operator=(const captcha_node &rhs) {
    _content = rhs._content;
    _node = rhs._node;
    return *this;
  }

  captcha_node &operator=(captcha_node &&rhs) noexcept {
    std::swap(_content, rhs._content);
    std::swap(_node, rhs._node);
    rhs._content = nullptr;
    rhs._node.clear();
    return *this;
  }

  template<typename Args>
  void add(const std::string &key, Args &&args) {
    captcha_node cd;
    node_helper nh(cd);
    nh.set(std::forward<Args>(args));
    _node[key] = cd;
  }

  captcha_node &operator[](const std::string &key) {
    return _node[key];
  }

  explicit operator int64_arg() {
    return *dynamic_cast<int64_arg *>(_content);
  }

  explicit operator string_arg() {
    return *dynamic_cast<string_arg *>(_content);
  }

  explicit operator enumerate() {
    return *dynamic_cast<enumerate *>(_content);
  }

  explicit operator bool_arg() {
    return *dynamic_cast<bool_arg *>(_content);
  }

 private:
  _base_arg *_content;
  std::map<std::string, captcha_node> _node;
};
}

#endif //XCAPTCHA_CAPTCHA_CONFIG_H
