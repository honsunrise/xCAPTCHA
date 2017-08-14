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
#include "iterator.h"

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

namespace detail {
class base_node;
class container_node;
class config_define_node;
class config_node;

typedef node_iterator_base<base_node> config_define_node_iterator;
typedef node_iterator_base<const base_node> const_config_define_node_iterator;

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

class base_node {
 public:
  virtual base_node *clone() = 0;
  virtual void clear() = 0;
};

class container_node : public base_node {
 public:
  container_node() {}

  container_node(const container_node &other) {
    for (auto i:other._sub_node) {
      _sub_node[i.first] = i.second->clone();
    }
  }

  container_node(container_node &&other) noexcept {
    _sub_node = std::move(other._sub_node);
  }

  virtual ~container_node() {
    clear();
  }

  container_node *clone() override {
    return new container_node(*this);
  };

  // size/iterator
  std::size_t size() const {
    return _sub_node.size();
  }

  const_config_define_node_iterator begin() const {
    return const_config_define_node_iterator(_sub_node.begin(), _sub_node.begin());
  }
  config_define_node_iterator begin() {
    return config_define_node_iterator(_sub_node.begin(), _sub_node.begin());
  }

  const_config_define_node_iterator end() const {
    return const_config_define_node_iterator(_sub_node.end(), _sub_node.end());
  }
  config_define_node_iterator end() {
    return config_define_node_iterator(_sub_node.end(), _sub_node.end());
  }

  bool insert(const std::string &key, base_node *node) {
    _sub_node.emplace(key, node);
    return true;
  }

  base_node *get(const std::string &key) {
    return _sub_node[key];
  }

  void assign(const container_node &rhs) {
    for (auto i:rhs._sub_node) {
      _sub_node[i.first] = i.second->clone();
    }
  }

  void assign(container_node &&rhs) {
    _sub_node = std::move(rhs._sub_node);
    rhs._sub_node.clear();
  }

  void clear() override {
    for (auto i : _sub_node) {
      delete i.second;
    }
  }

  std::map<std::string, base_node *> _sub_node;
};

class config_define_node : public base_node {
  class config_define_helper {
   public:
    config_define_helper(config_define_node *cd) : cd(cd) {}
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
    config_define_node *cd;
  };

 public:
  config_define_node() {
    _def_value = new content_holder<int>(0);
  }

  template<typename ValueType, typename... Args>
  explicit config_define_node(ValueType &&value, Args &&...args) {
    _def_value =
        new content_holder<typename std::decay<ValueType>::type>(std::forward<ValueType>(value));
    init_args(std::forward<Args>(args)...);
  }

  config_define_node(const config_define_node &other) {
    for (auto i:other._rules) {
      _rules.push_back(i->clone());
    }
    _def_value = other._def_value ? other._def_value->clone() : nullptr;
  }

  config_define_node(config_define_node &&other) noexcept {
    _rules = std::move(other._rules);
    _def_value = other._def_value;
    other._rules.clear();
    other._def_value = nullptr;
  }

  virtual ~config_define_node() {
    clear();
  }

  config_define_node &operator=(const config_define_node &rhs) {
    clear();
    for (auto i:rhs._rules) {
      _rules.push_back(i->clone());
    }
    _def_value = rhs._def_value ? rhs._def_value->clone() : nullptr;
  }

  config_define_node &operator=(config_define_node &&rhs) noexcept {
    clear();
    _rules = std::move(rhs._rules);
    _def_value = rhs._def_value;
    rhs._rules.clear();
    rhs._def_value = nullptr;
  }

  config_define_node *clone() override {
    return new config_define_node(*this);
  }

  void clear() override {
    for (auto i : _rules) {
      delete i;
    }
    delete _def_value;
  }

 private:
  void init_args() {}

  template<typename F, typename... Args>
  void init_args(F &&f_arg, Args &&... args) {
    config_define_helper adh(this);
    adh.set(std::forward<F>(f_arg));
    init_args(std::forward<Args>(args)...);
  }

  std::vector<_base_arg *> _rules;
  placeholder *_def_value;
};

class config_node : public base_node {
 public:
  config_node():_def_value(nullptr) {}

  template<typename ValueType, typename F>
  config_node(ValueType &&value, F &&f_arg) {
    _def_value =
        new content_holder<typename std::decay<ValueType>::type>(std::forward<typename std::decay<ValueType>::type>(
            value));
  }

  config_node(const config_node &other) {
    _def_value = other._def_value ? other._def_value->clone() : nullptr;
  }

  config_node(config_node &&other) noexcept {
    _def_value = other._def_value;
    other._def_value = nullptr;
  }

  virtual ~config_node() {
    clear();
  }

  config_node &operator=(const config_node &rhs) {
    clear();
    _def_value = rhs._def_value ? rhs._def_value->clone() : nullptr;
  }

  config_node &operator=(config_node &&rhs) noexcept {
    clear();
    _def_value = rhs._def_value;
    rhs._def_value = nullptr;
  }

  config_node *clone() override {
    return new config_node(*this);
  }

  void clear() override {
    delete _def_value;
  }

 private:
  placeholder *_def_value;
};
}

enum class config_define_node_type : char {
  CONTAINER,
  CONFIG_DEFINE,
};

class config_define {
 public:
  config_define() : _node(nullptr) {}

  explicit config_define(config_define_node_type type) {
    if (type == config_define_node_type::CONTAINER) {
      _node = new detail::container_node();
    } else if (type == config_define_node_type::CONFIG_DEFINE) {
      _node = new detail::config_define_node();
    }
  }

  explicit config_define(detail::base_node *node) : _node(node) {}

  template<typename ValueType, typename... Args>
  explicit config_define(ValueType &&value, Args &&...args) {
    _node = new detail::config_define_node(std::forward<ValueType>(value), std::forward<Args>(args)...);
  }

  config_define operator[](const std::string &key) {
    if (!_node) {

    }
    if (typeid(*_node) != typeid(detail::container_node)) {

    }
    auto *node = dynamic_cast<detail::container_node *>(_node);
    if (node->get(key))
      return config_define(node->get(key)->clone());
    return config_define();
  };

  bool insert(const std::string &key, config_define *define) {
    if (!_node) {

    }
    if (typeid(*_node) != typeid(detail::container_node)) {

    }
    auto *node = dynamic_cast<detail::container_node *>(_node);
    return node->insert(key, define->_node);
  }

 private:
  detail::base_node *_node;
};

enum class config_node_type : char {
  CONTAINER,
  CONFIG,
};

class config {
 public:
  config() : _node(nullptr) {}

  explicit config(config_node_type type) {
    if (type == config_node_type::CONTAINER) {
      _node = new detail::container_node();
    } else if (type == config_node_type::CONFIG) {
      _node = new detail::config_node();
    }
  }

  explicit config(detail::base_node *node) : _node(node) {}

  template<typename ValueType, typename... Args>
  explicit config(ValueType &&value, Args &&...args) {
    _node = new detail::config_define_node(std::forward(value), std::forward(args)...);
  }

  config operator[](const std::string &key) {
    if (!_node) {

    }
    if (typeid(*_node) != typeid(detail::container_node)) {

    }
    auto *node = dynamic_cast<detail::container_node *>(_node);
    if (node->get(key))
      return config(node->get(key)->clone());
    return config();
  };

  bool insert(const std::string &key, config *c) {
    if (!_node) {

    }
    if (typeid(*_node) != typeid(detail::container_node)) {

    }
    auto *node = dynamic_cast<detail::container_node *>(_node);
    return node->insert(key, c->_node);
  }

 private:
  detail::base_node *_node;
};

}

#endif //XCAPTCHA_CAPTCHA_CONFIG_H
