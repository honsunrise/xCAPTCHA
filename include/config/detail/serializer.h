//
// Created by zhsyourai on 8/19/17.
//

#ifndef XCAPTCHA_SERIALIZER_H
#define XCAPTCHA_SERIALIZER_H

#include <utility>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <forward_list>
#include <array>
#include <tuple>
#include "declaration.h"
#include "value_type.h"
#include "typed_constructor.h"

namespace captcha_config {
namespace detail {

template<typename BasicConfigType, typename T, utils::enable_if_t<
    std::is_same<T, typename BasicConfigType::boolean_t>::value, int> = 0>
void to_config(BasicConfigType &j, T b) noexcept {
  typed_constructor<value_t::boolean>::construct(j, b);
}

template<typename BasicConfigType, typename CompatibleString,
    utils::enable_if_t<std::is_constructible<typename BasicConfigType::string_t,
                                             CompatibleString>::value, int> = 0>
void to_config(BasicConfigType &j, const CompatibleString &s) {
  typed_constructor<value_t::string>::construct(j, s);
}

template<typename BasicConfigType>
void to_config(BasicConfigType &j, typename BasicConfigType::string_t &&s) {
  typed_constructor<value_t::string>::construct(j, std::move(s));
}

template<typename BasicConfigType, typename FloatType,
    utils::enable_if_t<std::is_floating_point<FloatType>::value, int> = 0>
void to_config(BasicConfigType &j, FloatType val) noexcept {
  typed_constructor<value_t::number_float>::construct(j,
                                                      static_cast<typename BasicConfigType::number_float_t>(val));
}

template<
    typename BasicConfigType, typename CompatibleNumberUnsignedType,
    utils::enable_if_t<utils::is_compatible_integer_type<typename BasicConfigType::number_unsigned_t,
                                                         CompatibleNumberUnsignedType>::value, int> = 0>
void to_config(BasicConfigType &j, CompatibleNumberUnsignedType val) noexcept {
  typed_constructor<value_t::number_unsigned>::construct(j,
                                                         static_cast<typename BasicConfigType::number_unsigned_t>(val));
}

template<
    typename BasicConfigType, typename CompatibleNumberIntegerType,
    utils::enable_if_t<utils::is_compatible_integer_type<typename BasicConfigType::number_integer_t,
                                                         CompatibleNumberIntegerType>::value, int> = 0>
void to_config(BasicConfigType &j, CompatibleNumberIntegerType val) noexcept {
  typed_constructor<value_t::number_integer>::construct(j,
                                                        static_cast<typename BasicConfigType::number_integer_t>(val));
}

template<typename BasicConfigType, typename EnumType,
    utils::enable_if_t<std::is_enum<EnumType>::value, int> = 0>
void to_config(BasicConfigType &j, EnumType e) noexcept {
  using underlying_type = typename std::underlying_type<EnumType>::type;
  typed_constructor<value_t::number_integer>::construct(j, static_cast<underlying_type>(e));
}

template<typename BasicConfigType>
void to_config(BasicConfigType &j, const std::vector<bool> &e) {
  typed_constructor<value_t::array>::construct(j, e);
}

template<
    typename BasicConfigType, typename CompatibleArrayType,
    utils::enable_if_t<
        utils::is_compatible_array_type<BasicConfigType, CompatibleArrayType>::value or
            std::is_same<typename BasicConfigType::array_t, CompatibleArrayType>::value,
        int> = 0>
void to_config(BasicConfigType &j, const CompatibleArrayType &arr) {
  typed_constructor<value_t::array>::construct(j, arr);
}

template<typename BasicConfigType>
void to_config(BasicConfigType &j, typename BasicConfigType::array_t &&arr) {
  typed_constructor<value_t::array>::construct(j, std::move(arr));
}

template<
    typename BasicConfigType, typename CompatibleObjectType,
    utils::enable_if_t<utils::is_compatible_map_type<BasicConfigType, CompatibleObjectType>::value,
        int> = 0>
void to_config(BasicConfigType &j, const CompatibleObjectType &obj) {
  typed_constructor<value_t::map>::construct(j, obj);
}

template<typename BasicConfigType>
void to_config(BasicConfigType &j, typename BasicConfigType::object_t &&obj) {
  typed_constructor<value_t::map>::construct(j, std::move(obj));
}

template<typename BasicConfigType, typename T, std::size_t N,
    utils::enable_if_t<not std::is_constructible<
        typename BasicConfigType::string_t, T (&)[N]>::value,
                       int> = 0>
void to_config(BasicConfigType &j, T (&arr)[N]) {
  typed_constructor<value_t::array>::construct(j, arr);
}

template<typename BasicConfigType, typename... Args>
void to_config(BasicConfigType &j, const std::pair<Args...> &p) {
  j = {p.first, p.second};
}

template<typename BasicConfigType, typename Tuple, std::size_t... Idx>
void to_config_tuple_impl(BasicConfigType &j, const Tuple &t, utils::index_sequence<Idx...>) {
  j = {std::get<Idx>(t)...};
}

template<typename BasicConfigType, typename... Args>
void to_config(BasicConfigType &j, const std::tuple<Args...> &t) {
  to_config_tuple_impl(j, t, utils::index_sequence_for<Args...> {});
}

// overloads for basic_json template parameters
template<typename BasicConfigType, typename ArithmeticType,
    utils::enable_if_t<std::is_arithmetic<ArithmeticType>::value and
        not std::is_same<ArithmeticType,
                         typename BasicConfigType::boolean_t>::value,
                       int> = 0>
void get_arithmetic_value(const BasicConfigType &j, ArithmeticType &val) {
  switch (static_cast<value_t>(j)) {
    case value_t::number_unsigned: {
      val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicConfigType::number_unsigned_t *>());
      break;
    }
    case value_t::number_integer: {
      val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicConfigType::number_integer_t *>());
      break;
    }
    case value_t::number_float: {
      val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicConfigType::number_float_t *>());
      break;
    }

    default:CONFIG_THROW(type_error::create(302, "type must be number, but is " + std::string(j.type_name())));
  }
}

template<typename BasicConfigType>
void from_config(const BasicConfigType &j, typename BasicConfigType::boolean_t &b) {
  if (CONFIG_UNLIKELY(not j.is_boolean())) {
    CONFIG_THROW(type_error::create(302, "type must be boolean, but is " + std::string(j.type_name())));
  }
  b = *j.template get_ptr<const typename BasicConfigType::boolean_t *>();
}

template<typename BasicConfigType>
void from_config(const BasicConfigType &j, typename BasicConfigType::string_t &s) {
  if (CONFIG_UNLIKELY(not j.is_string())) {
    CONFIG_THROW(type_error::create(302, "type must be string, but is " + std::string(j.type_name())));
  }
  s = *j.template get_ptr<const typename BasicConfigType::string_t *>();
}

template<typename BasicConfigType>
void from_config(const BasicConfigType &j, typename BasicConfigType::number_float_t &val) {
  get_arithmetic_value(j, val);
}

template<typename BasicConfigType>
void from_config(const BasicConfigType &j, typename BasicConfigType::number_unsigned_t &val) {
  get_arithmetic_value(j, val);
}

template<typename BasicConfigType>
void from_config(const BasicConfigType &j, typename BasicConfigType::number_integer_t &val) {
  get_arithmetic_value(j, val);
}

template<typename BasicConfigType, typename EnumType,
    utils::enable_if_t<std::is_enum<EnumType>::value, int> = 0>
void from_config(const BasicConfigType &j, EnumType &e) {
  typename std::underlying_type<EnumType>::type val;
  get_arithmetic_value(j, val);
  e = static_cast<EnumType>(val);
}

template<typename BasicConfigType>
void from_config(const BasicConfigType &j, typename BasicConfigType::array_t &arr) {
  if (CONFIG_UNLIKELY(not j.is_array())) {
    CONFIG_THROW(type_error::create(302, "type must be array, but is " + std::string(j.type_name())));
  }
  arr = *j.template get_ptr<const typename BasicConfigType::array_t *>();
}

// forward_list doesn't have an insert method
template<typename BasicConfigType, typename T, typename Allocator,
    utils::enable_if_t<std::is_convertible<BasicConfigType, T>::value, int> = 0>
void from_config(const BasicConfigType &j, std::forward_list<T, Allocator> &l) {
  if (CONFIG_UNLIKELY(not j.is_array())) {
    CONFIG_THROW(type_error::create(302, "type must be array, but is " + std::string(j.type_name())));
  }
  std::transform(j.rbegin(), j.rend(),
                 std::front_inserter(l), [](const BasicConfigType &i) {
        return i.template get<T>();
      });
}

template<typename BasicConfigType, typename CompatibleArrayType>
void
from_config_array_impl(const BasicConfigType &j, CompatibleArrayType &arr, utils::priority_tag<0> /*unused*/) {
  using std::end;

  std::transform(j.begin(), j.end(),
                 std::inserter(arr, end(arr)), [](const BasicConfigType &i) {
        // get<BasicConfigType>() returns *this, this won't call a from_config
        // method when value_type is BasicConfigType
        return i.template get<typename CompatibleArrayType::value_type>();
      });
}

template<typename BasicConfigType, typename CompatibleArrayType>
auto from_config_array_impl(const BasicConfigType &j, CompatibleArrayType &arr, utils::priority_tag<1> /*unused*/)
-> decltype(
arr.reserve(std::declval<typename CompatibleArrayType::size_type>()),
    void()) {
  using std::end;

  arr.reserve(j.size());
  std::transform(j.begin(), j.end(),
                 std::inserter(arr, end(arr)), [](const BasicConfigType &i) {
        // get<BasicConfigType>() returns *this, this won't call a from_config
        // method when value_type is BasicConfigType
        return i.template get<typename CompatibleArrayType::value_type>();
      });
}

template<typename BasicConfigType, typename T, std::size_t N>
void from_config_array_impl(const BasicConfigType &j, std::array<T, N> &arr, utils::priority_tag<2> /*unused*/) {
  for (std::size_t i = 0; i < N; ++i) {
    arr[i] = j.at(i).template get<T>();
  }
}

template<typename BasicConfigType, typename CompatibleArrayType,
    utils::enable_if_t<utils::is_compatible_array_type<BasicConfigType, CompatibleArrayType>::value and
        std::is_convertible<BasicConfigType, typename CompatibleArrayType::value_type>::value and
        not std::is_same<typename BasicConfigType::array_t, CompatibleArrayType>::value, int> = 0>
void from_config(const BasicConfigType &j, CompatibleArrayType &arr) {
  if (CONFIG_UNLIKELY(not j.is_array())) {
    CONFIG_THROW(type_error::create(302, "type must be array, but is " + std::string(j.type_name())));
  }

  from_config_array_impl(j, arr, utils::priority_tag<2> {});
}

template<typename BasicConfigType, typename CompatibleObjectType,
    utils::enable_if_t<utils::is_compatible_map_type<BasicConfigType, CompatibleObjectType>::value, int> = 0>
void from_config(const BasicConfigType &j, CompatibleObjectType &obj) {
  if (CONFIG_UNLIKELY(not j.is_object())) {
    CONFIG_THROW(type_error::create(302, "type must be map, but is " + std::string(j.type_name())));
  }

  auto inner_object = j.template get_ptr<const typename BasicConfigType::object_t *>();
  using value_type = typename CompatibleObjectType::value_type;
  std::transform(
      inner_object->begin(), inner_object->end(),
      std::inserter(obj, obj.begin()),
      [](typename BasicConfigType::object_t::value_type const &p) {
        return value_type(p.first, p.second.template get<typename CompatibleObjectType::mapped_type>());
      });
}

// overload for arithmetic types, not chosen for basic_json template arguments
// (BooleanType, etc..); note: Is it really necessary to provide explicit
// overloads for boolean_t etc. in case of a custom BooleanType which is not
// an arithmetic type?
template<typename BasicConfigType, typename ArithmeticType,
    utils::enable_if_t<
        std::is_arithmetic<ArithmeticType>::value and
            not std::is_same<ArithmeticType, typename BasicConfigType::number_unsigned_t>::value and
            not std::is_same<ArithmeticType, typename BasicConfigType::number_integer_t>::value and
            not std::is_same<ArithmeticType, typename BasicConfigType::number_float_t>::value and
            not std::is_same<ArithmeticType, typename BasicConfigType::boolean_t>::value,
        int> = 0>
void from_config(const BasicConfigType &j, ArithmeticType &val) {
  switch (static_cast<value_t>(j)) {
    case value_t::number_unsigned: {
      val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicConfigType::number_unsigned_t *>());
      break;
    }
    case value_t::number_integer: {
      val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicConfigType::number_integer_t *>());
      break;
    }
    case value_t::number_float: {
      val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicConfigType::number_float_t *>());
      break;
    }
    case value_t::boolean: {
      val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicConfigType::boolean_t *>());
      break;
    }

    default:CONFIG_THROW(type_error::create(302, "type must be number, but is " + std::string(j.type_name())));
  }
}

template<typename BasicConfigType, typename... Args>
void from_config(const BasicConfigType &j, std::pair<Args...> &p) {
  p = {j.at(0), j.at(1)};
}

template<typename BasicConfigType, typename Tuple, std::size_t... Idx>
void from_config_tuple_impl(const BasicConfigType &j, Tuple &t, utils::index_sequence<Idx...>) {
  t = std::make_tuple(j.at(Idx)...);
}

template<typename BasicConfigType, typename... Args>
void from_config(const BasicConfigType &j, std::tuple<Args...> &t) {
  from_config_tuple_impl(j, t, utils::index_sequence_for<Args...> {});
}

struct to_config_fn {
 private:
  template<typename BasicConfigType, typename T>
  auto call(BasicConfigType &j, T &&val, utils::priority_tag<1> /*unused*/) const noexcept(noexcept(to_config(j,
                                                                                                            std::forward<
                                                                                                                T>(
                                                                                                                val))))
  -> decltype(to_config(j, std::forward<T>(val)), void()) {
    return to_config(j, std::forward<T>(val));
  }

  template<typename BasicConfigType, typename T>
  void call(BasicConfigType & /*unused*/, T && /*unused*/, utils::priority_tag<0> /*unused*/) const noexcept {
    static_assert(sizeof(BasicConfigType) == 0,
                  "could not find to_config() method in T's namespace");
  }

 public:
  template<typename BasicConfigType, typename T>
  void operator()(BasicConfigType &j, T &&val) const
  noexcept(noexcept(std::declval<to_config_fn>().call(j, std::forward<T>(val),
                                                      utils::priority_tag < 1 > {}))) {
    return call(j, std::forward<T>(val), utils::priority_tag<1> {});
  }
};

struct from_config_fn {
 private:
  template<typename BasicConfigType, typename T>
  auto call(const BasicConfigType &j, T &val, utils::priority_tag<1> /*unused*/) const
  noexcept(noexcept(from_config(j, val)))
  -> decltype(from_config(j, val), void()) {
    return from_config(j, val);
  }

  template<typename BasicConfigType, typename T>
  void
  call(const BasicConfigType & /*unused*/, T & /*unused*/, utils::priority_tag<0> /*unused*/) const noexcept {
    static_assert(sizeof(BasicConfigType) == 0,
                  "could not find from_config() method in T's namespace");
  }

 public:
  template<typename BasicConfigType, typename T>
  void operator()(const BasicConfigType &j, T &val) const
  noexcept(noexcept(std::declval<from_config_fn>().call(j, val, utils::priority_tag < 1 > {}))) {
    return call(j, val, utils::priority_tag<1> {});
  }
};

template<typename T>
struct static_const {
  static constexpr T value{};
};

template<typename T>
constexpr T static_const<T>::value;

}

namespace {
constexpr const auto &to_config = detail::static_const<detail::to_config_fn>::value;
constexpr const auto &from_config = detail::static_const<detail::from_config_fn>::value;
}

template<typename, typename>
struct adl_serializer {
  template<typename BasicConfigType, typename ValueType>
  static void from_config(BasicConfigType &&j, ValueType &val) noexcept(
  noexcept(from_config(std::forward<BasicConfigType>(j), val))) {
    from_config(std::forward<BasicConfigType>(j), val);
  }

  template<typename BasicConfigType, typename ValueType>
  static void to_config(BasicConfigType &j, ValueType &&val) noexcept(
  noexcept(to_config(j, std::forward<ValueType>(val)))) {
    to_config(j, std::forward<ValueType>(val));
  }
};
}
#endif //XCAPTCHA_SERIALIZER_H
