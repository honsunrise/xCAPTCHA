//
// Created by zhsyourai on 8/7/17.
//

#ifndef XCAPTCHA_CAPTCHA_CONFIG_H
#define XCAPTCHA_CAPTCHA_CONFIG_H
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <typeinfo>
#include <boost/core/noncopyable.hpp>

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

class placeholder : boost::noncopyable {
 public:
  virtual ~placeholder() {}
 public:
  virtual const std::type_info &type() const noexcept = 0;
  virtual placeholder *clone() const = 0;
};

template<typename ValueType>
class content_holder : public placeholder {
 public:
  content_holder(const ValueType &value) : held(value) {
  }

  content_holder(ValueType &&value) noexcept : held(std::move(value)) {
  }

 public:
  const std::type_info &type() const noexcept {
    return typeid(ValueType);
  }

  content_holder *clone() const {
    return new content_holder(held);
  }

 public:
  ValueType held;
};

enum node_type {
  CONTAINER,
  VALUE
};

class config_define {
  class config_define_helper {
   public:
    config_define_helper(config_define *cd) : cd(cd) {}
    void set(regex &&v) {
      cd->_rules.push_back(new regex(v));
    }
    void set(minimum &&v) {
      cd->_rules.push_back(new minimum(v));
    }
    void set(maximum &&v) {
      cd->_rules.push_back(new maximum(v));
    }
    void set(enumerate &&v) {
      cd->_rules.push_back(new enumerate(v));
    }
   private:
    config_define *cd;
  };

 public:
  template<typename ValueType, typename F, typename... Args>
  config_define(ValueType &&value, F &&f_arg, Args &&...args) {
    _type = VALUE;
    _def_value =
        new content_holder<typename std::decay<ValueType>::type>(std::forward<typename std::decay<ValueType>::type>(
            value));
    init_args(std::forward<F>(f_arg), std::forward<Args>(args)...);
  }

  config_define(const config_define &other) {
    _type = other._type;
    if (_type == VALUE) {
      for (auto i:other._rules) {
        _rules.push_back(i->clone());
      }
      _def_value = other._def_value ? other._def_value->clone() : nullptr;
    } else if (_type == CONTAINER) {
      for (auto i:other._sub_node) {
        _sub_node[i.first] = new config_define(*i.second);
      }
    }
  }

  config_define(config_define &&other) noexcept {
    _type = other._type;
    if (_type == VALUE) {
      _rules = std::move(other._rules);
      _def_value = other._def_value;
      other._rules.clear();
      other._def_value = nullptr;
    } else if (_type == CONTAINER) {
      _sub_node = std::move(other._sub_node);
      other._sub_node.clear();
    }
  }

  virtual ~config_define() {
    clear();
  }

  config_define &operator=(const config_define &rhs) {
    clear();
    _type = rhs._type;
    if (_type == VALUE) {
      for (auto i:rhs._rules) {
        _rules.push_back(i->clone());
      }
      _def_value = rhs._def_value ? rhs._def_value->clone() : nullptr;
    } else if (_type == CONTAINER) {
      for (auto i:rhs._sub_node) {
        _sub_node[i.first] = new config_define(*i.second);
      }
    }
  }

  config_define &operator=(config_define &&rhs) noexcept {
    clear();
    if (_type == VALUE) {
      _rules = std::move(rhs._rules);
      _def_value = rhs._def_value;
      rhs._rules.clear();
      rhs._def_value = nullptr;
    } else if (_type == CONTAINER) {
      _sub_node = std::move(rhs._sub_node);
      rhs._sub_node.clear();
    }
  }

  template<typename KeyType, typename ValueType>
  bool insert(KeyType &&key, ValueType &&cd) {
    if (_type == CONTAINER) {
      _sub_node.insert(std::pair<typename std::decay<KeyType>::type, typename std::decay<ValueType>::type>(std::forward(
          key), new config_define(std::forward(cd))));
    }
  }

  config_define &operator[](const std::string &key) {
    if (_type == CONTAINER) {
      return *_sub_node[key];
    }
    assert(true);
  }

  config_define &operator[](std::string &&key) {
    if (_type == CONTAINER) {
      return *_sub_node[std::move(key)];
    }
    assert(true);
  }

 private:
  void clear() {
    if (_type == VALUE) {
      for (auto i : _rules) {
        delete i;
      }
      delete _def_value;
    } else if (_type == CONTAINER) {
      for (auto i : _sub_node) {
        delete i.second;
      }
    }
  }

  void init_args() {}

  template<typename F, typename... Args>
  void init_args(F &&f_arg, Args &&... args) {
    config_define_helper adh(this);
    adh.set(std::forward<F>(f_arg));
    init_args(std::forward<Args>(args)...);
  }
  node_type _type;
  std::vector<_base_arg *> _rules;
  placeholder *_def_value;
  std::map<std::string, config_define *> _sub_node;
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
