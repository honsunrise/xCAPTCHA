#ifndef XCAPTCHA_CAPTCHA_DETAIL_ITERATOR_H
#define XCAPTCHA_CAPTCHA_DETAIL_ITERATOR_H

#include <cstddef>
#include <iterator>
#include <memory>
#include <map>
#include <utility>
#include <vector>

namespace captcha_config {
namespace detail {
template<typename V>
struct node_iterator_value : public std::pair<std::string, V *> {
  typedef std::pair<std::string, V *> kv;

  node_iterator_value() : kv() {}
  explicit node_iterator_value(const std::string &key, V &value) : kv(key, &value) {}
};

template<typename V>
class node_iterator_base : public std::iterator<std::forward_iterator_tag, V, std::ptrdiff_t, V *, V> {
 public:
  typedef typename std::decay<V>::type type;
  typedef std::map<std::string, type *> node_map;
  typedef node_iterator_value<type> value_type;

 private:
  template<typename W>
  struct node_iterator_type {
    typedef typename node_map::iterator map;
  };

  template<typename W>
  struct node_iterator_type<const W> {
    typedef typename node_map::const_iterator map;
  };

 public:
  typedef typename node_iterator_type<V>::map MapIter;

 private:
  struct enabler {};

  struct proxy {
    explicit proxy(const node_iterator_value<type> &x) : m_ref(x) {}
    node_iterator_value<type> *operator->() { return std::addressof(m_ref); }
    explicit operator node_iterator_value<type> *() { return std::addressof(m_ref); }

    node_iterator_value<type> m_ref;
  };

 public:
  node_iterator_base() : m_mapIt() {}

  explicit node_iterator_base(MapIter mapIt) : m_mapIt(mapIt) {}

  template<typename W>
  explicit node_iterator_base(const node_iterator_base<W> &rhs,
                              typename std::enable_if<std::is_convertible<W *, type *>::value,
                                                      enabler>::type = enabler()) : m_mapIt(rhs.m_mapIt) {}

  template<typename>
  friend
  class iterator_base;

  template<typename W>
  bool operator==(const node_iterator_base<W> &rhs) const {
    return m_mapIt == rhs.m_mapIt;
  }

  template<typename W>
  bool operator!=(const node_iterator_base<W> &rhs) const {
    return !(*this == rhs);
  }

  node_iterator_base<V> &operator++() {
    ++m_mapIt;
    return *this;
  }

  node_iterator_base<V> operator++(int) {
    node_iterator_base<V> iterator_pre(*this);
    ++(*this);
    return iterator_pre;
  }

  value_type operator*() const {
    return value_type(m_mapIt->first, *m_mapIt->second);
  }

  proxy operator->() const { return proxy(**this); }

 private:
  MapIter m_mapIt;
};
}
}

#endif  // XCAPTCHA_CAPTCHA_DETAIL_ITERATOR_H
