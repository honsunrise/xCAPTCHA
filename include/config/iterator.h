#ifndef XCAPTCHA_CAPTCHA_ITERATOR_H
#define XCAPTCHA_CAPTCHA_ITERATOR_H

#include <cstddef>
#include <iterator>
#include <memory>
#include <map>
#include <utility>
#include <vector>
#include "config/detail/config.h"

namespace captcha_config {

template<typename V>
struct iterator_value : public std::pair<std::string, V> {
  typedef std::pair<std::string, V> kv;

  iterator_value() : kv() {}
  explicit iterator_value(const std::string &key, V &value) : kv(key, value) {}
};

template<typename V>
class iterator_base : public std::iterator<std::forward_iterator_tag, V, std::ptrdiff_t, V *, V> {
 public:
  typedef iterator_value<V> value_type;

 private:
  struct enabler {};

  struct proxy {
    explicit proxy(const iterator_value<V> &x) : m_ref(x) {}
    iterator_value<V> *operator->() { return std::addressof(m_ref); }
    explicit operator iterator_value<V> *() { return std::addressof(m_ref); }

    iterator_value<V> m_ref;
  };

 public:
  iterator_base() : m_mapIt() {}

  explicit iterator_base(detail::container_node::iterator mapIt) : m_mapIt(mapIt) {}

  template<typename W>
  explicit iterator_base(const iterator_base<W> &rhs,
                              typename std::enable_if<std::is_convertible<W *, V *>::value,
                                                      enabler>::type = enabler()) : m_mapIt(rhs.m_mapIt) {}

  template<typename W>
  bool operator==(const iterator_base<W> &rhs) const {
    return m_mapIt == rhs.m_mapIt;
  }

  template<typename W>
  bool operator!=(const iterator_base<W> &rhs) const {
    return !(*this == rhs);
  }

  iterator_base<V> &operator++() {
    ++m_mapIt;
    return *this;
  }

  iterator_base<V> operator++(int) {
    iterator_base<V> iterator_pre(*this);
    ++(*this);
    return iterator_pre;
  }

  value_type operator*() const {
    return value_type(m_mapIt->first, V(m_mapIt->second));
  }

  proxy operator->() const { return proxy(**this); }

 private:
  detail::container_node::iterator m_mapIt;
};

}

#endif  // XCAPTCHA_CAPTCHA_ITERATOR_H
