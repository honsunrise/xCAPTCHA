#ifndef XCAPTCHA_CAPTCHA_DETAIL_ITERATOR_H
#define XCAPTCHA_CAPTCHA_DETAIL_ITERATOR_H

#include <cstddef>
#include <iterator>
#include <memory>
#include <map>
#include <utility>
#include <vector>
#include <cassert>
#include "utils.h"
#include "value_type.h"

namespace captcha_config {
namespace detail {

class primitive_iterator_t {
 public:
  using difference_type = std::ptrdiff_t;

  constexpr difference_type get_value() const noexcept {
    return m_it;
  }

  void set_begin() noexcept {
    m_it = begin_value;
  }

  void set_end() noexcept {
    m_it = end_value;
  }

  constexpr bool is_begin() const noexcept {
    return (m_it == begin_value);
  }

  constexpr bool is_end() const noexcept {
    return (m_it == end_value);
  }

  friend constexpr bool operator==(primitive_iterator_t lhs, primitive_iterator_t rhs) noexcept {
    return (lhs.m_it == rhs.m_it);
  }

  friend constexpr bool operator!=(primitive_iterator_t lhs, primitive_iterator_t rhs) noexcept {
    return not(lhs == rhs);
  }

  friend constexpr bool operator<(primitive_iterator_t lhs, primitive_iterator_t rhs) noexcept {
    return lhs.m_it < rhs.m_it;
  }

  friend constexpr bool operator<=(primitive_iterator_t lhs, primitive_iterator_t rhs) noexcept {
    return lhs.m_it <= rhs.m_it;
  }

  friend constexpr bool operator>(primitive_iterator_t lhs, primitive_iterator_t rhs) noexcept {
    return lhs.m_it > rhs.m_it;
  }

  friend constexpr bool operator>=(primitive_iterator_t lhs, primitive_iterator_t rhs) noexcept {
    return lhs.m_it >= rhs.m_it;
  }

  primitive_iterator_t operator+(difference_type i) {
    auto result = *this;
    result += i;
    return result;
  }

  friend constexpr difference_type operator-(primitive_iterator_t lhs, primitive_iterator_t rhs) noexcept {
    return lhs.m_it - rhs.m_it;
  }

  friend std::ostream &operator<<(std::ostream &os, primitive_iterator_t it) {
    return os << it.m_it;
  }

  primitive_iterator_t &operator++() {
    ++m_it;
    return *this;
  }

  primitive_iterator_t operator++(int) {
    auto result = *this;
    m_it++;
    return result;
  }

  primitive_iterator_t &operator--() {
    --m_it;
    return *this;
  }

  primitive_iterator_t operator--(int) {
    auto result = *this;
    m_it--;
    return result;
  }

  primitive_iterator_t &operator+=(difference_type n) {
    m_it += n;
    return *this;
  }

  primitive_iterator_t &operator-=(difference_type n) {
    m_it -= n;
    return *this;
  }

 private:
  static constexpr difference_type begin_value = 0;
  static constexpr difference_type end_value = begin_value + 1;

  difference_type m_it = std::numeric_limits<std::ptrdiff_t>::denorm_min();
};

template<typename ConfigType>
struct internal_iterator {
  typename ConfigType::map_t::iterator map_iterator{};
  typename ConfigType::array_t::iterator array_iterator{};
  primitive_iterator_t primitive_iterator{};
};

template<typename IteratorType> class iteration_proxy;

template<typename ConfigType>
class iter_impl : public std::iterator<std::random_access_iterator_tag, ConfigType> {
  friend ConfigType;
  friend iteration_proxy<iter_impl>;

  using map_t = typename ConfigType::map_t;
  using array_t = typename ConfigType::array_t;
  // make sure ConfigType is basic_config or const basic_config
  static_assert(utils::is_basic_config<typename std::remove_const<ConfigType>::type>::value,
                "iter_impl only accepts (const) basic_config");

 public:
  using value_type = typename ConfigType::value_type;
  using difference_type = typename ConfigType::difference_type;
  using pointer = typename std::conditional<std::is_const<ConfigType>::value,
                                            typename ConfigType::const_pointer,
                                            typename ConfigType::pointer>::type;
  using reference =
  typename std::conditional<std::is_const<ConfigType>::value,
                            typename ConfigType::const_reference,
                            typename ConfigType::reference>::type;
  using iterator_category = std::random_access_iterator_tag;

  iter_impl() = default;

  explicit iter_impl(pointer object) noexcept : m_object(object) {
    assert(m_object != nullptr);

    switch (m_object->m_type) {
      case value_t::map: {
        m_it.map_iterator = typename map_t::iterator();
        break;
      }

      case value_t::array: {
        m_it.array_iterator = typename array_t::iterator();
        break;
      }

      default: {
        m_it.primitive_iterator = primitive_iterator_t();
        break;
      }
    }
  }

  iter_impl(const iter_impl<typename std::remove_const<ConfigType>::type> &other) noexcept
      : m_object(other.m_object), m_it(other.m_it) {}

  iter_impl &operator=(const iter_impl<typename std::remove_const<ConfigType>::type> &other) noexcept {
    m_object = other.m_object;
    m_it = other.m_it;
    return *this;
  }

 private:
  void set_begin() noexcept {
    assert(m_object != nullptr);

    switch (m_object->m_type) {
      case value_t::map: {
        m_it.map_iterator = m_object->m_value.map->begin();
        break;
      }

      case value_t::array: {
        m_it.array_iterator = m_object->m_value.array->begin();
        break;
      }

      case value_t::null: {
        // set to end so begin()==end() is true: null is empty
        m_it.primitive_iterator.set_end();
        break;
      }

      default: {
        m_it.primitive_iterator.set_begin();
        break;
      }
    }
  }

  void set_end() noexcept {
    assert(m_object != nullptr);

    switch (m_object->m_type) {
      case value_t::map: {
        m_it.map_iterator = m_object->m_value.map->end();
        break;
      }

      case value_t::array: {
        m_it.array_iterator = m_object->m_value.array->end();
        break;
      }

      default: {
        m_it.primitive_iterator.set_end();
        break;
      }
    }
  }

 public:
  reference operator*() const {
    assert(m_object != nullptr);

    switch (m_object->m_type) {
      case value_t::map: {
        assert(m_it.map_iterator != m_object->m_value.map->end());
        return m_it.map_iterator->second;
      }

      case value_t::array: {
        assert(m_it.array_iterator != m_object->m_value.array->end());
        return *m_it.array_iterator;
      }

      case value_t::null: {
        JSON_THROW(invalid_iterator::create(214, "cannot get value"));
      }

      default: {
        if (JSON_LIKELY(m_it.primitive_iterator.is_begin())) {
          return *m_object;
        }

        JSON_THROW(invalid_iterator::create(214, "cannot get value"));
      }
    }
  }

  pointer operator->() const {
    assert(m_object != nullptr);

    switch (m_object->m_type) {
      case value_t::map: {
        assert(m_it.map_iterator != m_object->m_value.map->end());
        return &(m_it.map_iterator->second);
      }

      case value_t::array: {
        assert(m_it.array_iterator != m_object->m_value.array->end());
        return &*m_it.array_iterator;
      }

      default: {
        if (JSON_LIKELY(m_it.primitive_iterator.is_begin())) {
          return m_object;
        }

        JSON_THROW(invalid_iterator::create(214, "cannot get value"));
      }
    }
  }

  iter_impl operator++(int) {
    auto result = *this;
    ++(*this);
    return result;
  }

  iter_impl &operator++() {
    assert(m_object != nullptr);

    switch (m_object->m_type) {
      case value_t::map: {
        std::advance(m_it.map_iterator, 1);
        break;
      }

      case value_t::array: {
        std::advance(m_it.array_iterator, 1);
        break;
      }

      default: {
        ++m_it.primitive_iterator;
        break;
      }
    }

    return *this;
  }

  iter_impl operator--(int) {
    auto result = *this;
    --(*this);
    return result;
  }

  iter_impl &operator--() {
    assert(m_object != nullptr);

    switch (m_object->m_type) {
      case value_t::map: {
        std::advance(m_it.map_iterator, -1);
        break;
      }

      case value_t::array: {
        std::advance(m_it.array_iterator, -1);
        break;
      }

      default: {
        --m_it.primitive_iterator;
        break;
      }
    }

    return *this;
  }

  bool operator==(const iter_impl &other) const {
    // if objects are not the same, the comparison is undefined
    if (JSON_UNLIKELY(m_object != other.m_object)) {
      JSON_THROW(invalid_iterator::create(212, "cannot compare iterators of different containers"));
    }

    assert(m_object != nullptr);

    switch (m_object->m_type) {
      case value_t::map:return (m_it.map_iterator == other.m_it.map_iterator);

      case value_t::array:return (m_it.array_iterator == other.m_it.array_iterator);

      default:return (m_it.primitive_iterator == other.m_it.primitive_iterator);
    }
  }

  bool operator!=(const iter_impl &other) const {
    return not operator==(other);
  }

  bool operator<(const iter_impl &other) const {
    // if objects are not the same, the comparison is undefined
    if (JSON_UNLIKELY(m_object != other.m_object)) {
      JSON_THROW(invalid_iterator::create(212, "cannot compare iterators of different containers"));
    }

    assert(m_object != nullptr);

    switch (m_object->m_type) {
      case value_t::map:JSON_THROW(invalid_iterator::create(213, "cannot compare order of map iterators"));

      case value_t::array:return (m_it.array_iterator < other.m_it.array_iterator);

      default:return (m_it.primitive_iterator < other.m_it.primitive_iterator);
    }
  }

  bool operator<=(const iter_impl &other) const {
    return not other.operator<(*this);
  }

  bool operator>(const iter_impl &other) const {
    return not operator<=(other);
  }

  bool operator>=(const iter_impl &other) const {
    return not operator<(other);
  }

  iter_impl &operator+=(difference_type i) {
    assert(m_object != nullptr);

    switch (m_object->m_type) {
      case value_t::map:std::advance(m_it.map_iterator, i);
        break;

      case value_t::array: {
        std::advance(m_it.array_iterator, i);
        break;
      }

      default: {
        m_it.primitive_iterator += i;
        break;
      }
    }

    return *this;
  }

  iter_impl &operator-=(difference_type i) {
    return operator+=(-i);
  }

  iter_impl operator+(difference_type i) const {
    auto result = *this;
    result += i;
    return result;
  }

  friend iter_impl operator+(difference_type i, const iter_impl &it) {
    auto result = it;
    result += i;
    return result;
  }

  iter_impl operator-(difference_type i) const {
    auto result = *this;
    result -= i;
    return result;
  }

  difference_type operator-(const iter_impl &other) const {
    assert(m_object != nullptr);

    switch (m_object->m_type) {
      case value_t::map: {
        difference_type result = 0;
        for (auto it = other; it != *this; ++it, ++result);
        return result;
      }

      case value_t::array:return m_it.array_iterator - other.m_it.array_iterator;

      default:return m_it.primitive_iterator - other.m_it.primitive_iterator;
    }
  }

  reference operator[](difference_type n) const {
    assert(m_object != nullptr);

    switch (m_object->m_type) {
      case value_t::map: {
        auto it = *this;
        std::advance(it.m_it.map_iterator, n);
        return *it;
      }

      case value_t::array:return *std::next(m_it.array_iterator, n);

      case value_t::null:JSON_THROW(invalid_iterator::create(214, "cannot get value"));

      default: {
        if (JSON_LIKELY(m_it.primitive_iterator.get_value() == -n)) {
          return *m_object;
        }

        JSON_THROW(invalid_iterator::create(214, "cannot get value"));
      }
    }
  }

  typename map_t::key_type key() const {
    assert(m_object != nullptr);

    if (JSON_LIKELY(m_object->is_map())) {
      return m_it.map_iterator->first;
    }

    JSON_THROW(invalid_iterator::create(207, "cannot use key() for non-map iterators"));
  }

  reference value() const {
    return operator*();
  }

 private:
  pointer m_object = nullptr;
  internal_iterator<typename std::remove_const<ConfigType>::type> m_it = {};
};

template<typename IteratorType>
class iteration_proxy {
 private:
  /// helper class for iteration
  class iteration_proxy_internal {
   private:
    /// the iterator
    IteratorType anchor;
    /// an index for arrays (used to create key names)
    std::size_t array_index = 0;

   public:
    explicit iteration_proxy_internal(IteratorType it) noexcept : anchor(it) {}

    /// dereference operator (needed for range-based for)
    iteration_proxy_internal &operator*() {
      return *this;
    }

    /// increment operator (needed for range-based for)
    iteration_proxy_internal &operator++() {
      ++anchor;
      ++array_index;

      return *this;
    }

    /// inequality operator (needed for range-based for)
    bool operator!=(const iteration_proxy_internal &o) const noexcept {
      return anchor != o.anchor;
    }

    /// return key of the iterator
    std::string key() const {
      assert(anchor.m_object != nullptr);

      switch (anchor.m_object->type()) {
        // use integer array index as key
        case value_t::array:return std::to_string(array_index);

          // use key from the object
        case value_t::map:return anchor.key();

          // use an empty key for all primitive types
        default:return "";
      }
    }

    /// return value of the iterator
    typename IteratorType::reference value() const {
      return anchor.value();
    }
  };

  /// the container to iterate
  typename IteratorType::reference container;

 public:
  /// construct iteration proxy from a container
  explicit iteration_proxy(typename IteratorType::reference cont)
      : container(cont) {}

  /// return iterator begin (needed for range-based for)
  iteration_proxy_internal begin() noexcept {
    return iteration_proxy_internal(container.begin());
  }

  /// return iterator end (needed for range-based for)
  iteration_proxy_internal end() noexcept {
    return iteration_proxy_internal(container.end());
  }
};

template<typename Base>
class config_reverse_iterator : public std::reverse_iterator<Base> {
 public:
  using difference_type = std::ptrdiff_t;
  /// shortcut to the reverse iterator adaptor
  using base_iterator = std::reverse_iterator<Base>;
  /// the reference type for the pointed-to element
  using reference = typename Base::reference;

  /// create reverse iterator from iterator
  config_reverse_iterator(const typename base_iterator::iterator_type &it) noexcept
      : base_iterator(it) {}

  /// create reverse iterator from base class
  config_reverse_iterator(const base_iterator &it) noexcept : base_iterator(it) {}

  /// post-increment (it++)
  config_reverse_iterator operator++(int) {
    return static_cast<config_reverse_iterator>(base_iterator::operator++(1));
  }

  /// pre-increment (++it)
  config_reverse_iterator &operator++() {
    return static_cast<config_reverse_iterator &>(base_iterator::operator++());
  }

  /// post-decrement (it--)
  config_reverse_iterator operator--(int) {
    return static_cast<config_reverse_iterator>(base_iterator::operator--(1));
  }

  /// pre-decrement (--it)
  config_reverse_iterator &operator--() {
    return static_cast<config_reverse_iterator &>(base_iterator::operator--());
  }

  /// add to iterator
  config_reverse_iterator &operator+=(difference_type i) {
    return static_cast<config_reverse_iterator &>(base_iterator::operator+=(i));
  }

  /// add to iterator
  config_reverse_iterator operator+(difference_type i) const {
    return static_cast<config_reverse_iterator>(base_iterator::operator+(i));
  }

  /// subtract from iterator
  config_reverse_iterator operator-(difference_type i) const {
    return static_cast<config_reverse_iterator>(base_iterator::operator-(i));
  }

  /// return difference
  difference_type operator-(const config_reverse_iterator &other) const {
    return base_iterator(*this) - base_iterator(other);
  }

  /// access to successor
  reference operator[](difference_type n) const {
    return *(this->operator+(n));
  }

  /// return the key of an object iterator
  auto key() const -> decltype(std::declval<Base>().key()) {
    auto it = --this->base();
    return it.key();
  }

  /// return the value of an iterator
  reference value() const {
    auto it = --this->base();
    return it.operator*();
  }
};

}
}

#endif  // XCAPTCHA_CAPTCHA_DETAIL_ITERATOR_H
