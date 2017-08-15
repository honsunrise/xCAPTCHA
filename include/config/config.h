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


enum class config_define_node_type : char {
  CONTAINER,
  CONFIG_DEFINE,
};

template<typename V>
struct config_base_iterator_value : public std::pair<std::string, V *> {
  typedef std::pair<std::string, V *> kv;

  config_base_iterator_value() : kv() {}
  explicit config_base_iterator_value(const std::string &key, V &value) : kv(key, &value) {}
};

template<typename V>
class config_base_iterator : public std::iterator<std::forward_iterator_tag,
                                                  config_base_iterator_value<typename std::decay<V>::type>,
                                                  std::ptrdiff_t,
                                                  config_base_iterator_value<typename std::decay<V>::type> *,
                                                  config_base_iterator_value<typename std::decay<V>::type>> {
 public:
  typedef typename std::decay<V>::type type;
  typedef std::map<std::string, type *> node_map;
  typedef config_base_iterator_value<type> value_type;

  template<typename W>
  struct node_iterator_type {
    typedef typename node_map::iterator map;
  };

  template<typename W>
  struct node_iterator_type<const W> {
    typedef typename node_map::const_iterator map;
  };
  typedef typename node_iterator_type<V>::map MapIter;

 private:
  struct enabler {};

  struct proxy {
    explicit proxy(const config_base_iterator_value<type> &x) : m_ref(x) {}
    config_base_iterator_value<type> *operator->() { return std::addressof(m_ref); }
    explicit operator config_base_iterator_value<type> *() { return std::addressof(m_ref); }

    config_base_iterator_value<type> m_ref;
  };

 public:
  config_base_iterator() : m_mapIt(), m_mapEnd() {}

  explicit config_base_iterator(MapIter
                                mapIt,
                                MapIter mapEnd) : m_mapIt(mapIt), m_mapEnd(mapEnd) {}

  template<typename W>
  explicit config_base_iterator(const config_base_iterator <W> &rhs,
                              typename std::enable_if<std::is_convertible<W *, type *>::value,
                                                      enabler>::type = enabler())
      : m_mapIt(rhs.m_mapIt),
        m_mapEnd(rhs.m_mapEnd) {}

  template<typename>
  friend
  class node_iterator_base;

  template<typename W>
  bool operator==(const config_base_iterator<W> &rhs) const {
    return m_mapIt == rhs.m_mapIt;
  }

  template<typename W>
  bool operator!=(const config_base_iterator<W> &rhs) const {
    return !(*this == rhs);
  }

  config_base_iterator<type> &operator++() {
    ++m_mapIt;
    return *this;
  }

  config_base_iterator<type> operator++(int) {
    config_base_iterator < type > iterator_pre(*this);
    ++(*this);
    return iterator_pre;
  }

  value_type operator*() const {
    return value_type(m_mapIt->first, *m_mapIt->second);
  }

  proxy operator->() const { return proxy(**this); }

 private:
  detail::config_define_node_iterator
};

class config_define;

typedef config_base_iterator<config_define> config_define_iterator;
typedef config_base_iterator<const config_define> const_config_define_iterator;

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

  const_config_define_iterator begin() const {
    if (!_node) {

    }
    if (typeid(*_node) != typeid(detail::container_node)) {

    }
    auto *node = dynamic_cast<detail::container_node *>(_node);
    return const_config_define_iterator(node->begin());
  }

  config_define_iterator begin() {
    if (!_node) {

    }
    if (typeid(*_node) != typeid(detail::container_node)) {

    }
    auto *node = dynamic_cast<detail::container_node *>(_node);
    return config_define_iterator(node->begin());
  }

  const_config_define_iterator end() const {
    if (!_node) {

    }
    if (typeid(*_node) != typeid(detail::container_node)) {

    }
    auto *node = dynamic_cast<detail::container_node *>(_node);
    return const_config_define_iterator(node->end());
  }

  config_define_iterator end() {
    if (!_node) {

    }
    if (typeid(*_node) != typeid(detail::container_node)) {

    }
    auto *node = dynamic_cast<detail::container_node *>(_node);
    return config_define_iterator(node->end());
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
