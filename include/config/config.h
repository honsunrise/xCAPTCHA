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
#include "config/detail/config.h"
#include "rules.h"
#include "iterator.h"

namespace captcha_config {

class config_define {
 public:
  typedef iterator_base<config_define> iterator;
  typedef iterator_base<const config_define> const_iterator;
  typedef size_t size_type;

  enum class config_define_node_type : char {
    CONTAINER,
    CONFIG_DEFINE,
  };
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

  template <typename T> T as() const {
    if (!_node) {

    }
    if (typeid(*_node) != typeid(detail::config_define_node)) {

    }
    auto *node = dynamic_cast<detail::config_define_node *>(_node);
    node->as<T>();
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

  const_iterator begin() const {
    if (!_node) {

    }
    if (typeid(*_node) != typeid(detail::container_node)) {

    }
    auto *node = dynamic_cast<detail::container_node *>(_node);
    return const_iterator(node->begin());
  }

  iterator begin() {
    if (!_node) {

    }
    if (typeid(*_node) != typeid(detail::container_node)) {

    }
    auto *node = dynamic_cast<detail::container_node *>(_node);
    return iterator(node->begin());
  }

  const_iterator end() const {
    if (!_node) {

    }
    if (typeid(*_node) != typeid(detail::container_node)) {

    }
    auto *node = dynamic_cast<detail::container_node *>(_node);
    return const_iterator(node->end());
  }

  iterator end() {
    if (!_node) {

    }
    if (typeid(*_node) != typeid(detail::container_node)) {

    }
    auto *node = dynamic_cast<detail::container_node *>(_node);
    return iterator(node->end());
  }

 private:
  detail::base_node *_node;
};

class config {
 public:
  enum class config_node_type : char {
    CONTAINER,
    CONFIG,
  };

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
