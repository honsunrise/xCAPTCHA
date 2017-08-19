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
#include "value_type.h"
#include "typed_constructor.h"
namespace captcha_config {
namespace detail {

template<typename BasicJsonType, typename T, utils::enable_if_t<
    std::is_same<T, typename BasicJsonType::boolean_t>::value, int> = 0>
void to_json(BasicJsonType &j, T b) noexcept {
  typed_constructor<value_t::boolean>::construct(j, b);
}

template<typename BasicJsonType, typename CompatibleString,
    utils::enable_if_t<std::is_constructible<typename BasicJsonType::string_t,
                                      CompatibleString>::value, int> = 0>
void to_json(BasicJsonType &j, const CompatibleString &s) {
  typed_constructor<value_t::string>::construct(j, s);
}

template<typename BasicJsonType>
void to_json(BasicJsonType &j, typename BasicJsonType::string_t &&s) {
  typed_constructor<value_t::string>::construct(j, std::move(s));
}

template<typename BasicJsonType, typename FloatType,
    utils::enable_if_t<std::is_floating_point<FloatType>::value, int> = 0>
void to_json(BasicJsonType &j, FloatType val) noexcept {
  typed_constructor<value_t::number_float>::construct(j, static_cast<typename BasicJsonType::number_float_t>(val));
}

template<
    typename BasicJsonType, typename CompatibleNumberUnsignedType,
    utils::enable_if_t<utils::is_compatible_integer_type<typename BasicJsonType::number_unsigned_t,
                                           CompatibleNumberUnsignedType>::value, int> = 0>
void to_json(BasicJsonType &j, CompatibleNumberUnsignedType val) noexcept {
  typed_constructor<value_t::number_unsigned>::construct(j,
                                                            static_cast<typename BasicJsonType::number_unsigned_t>(val));
}

template<
    typename BasicJsonType, typename CompatibleNumberIntegerType,
    utils::enable_if_t<utils::is_compatible_integer_type<typename BasicJsonType::number_integer_t,
                                           CompatibleNumberIntegerType>::value, int> = 0>
void to_json(BasicJsonType &j, CompatibleNumberIntegerType val) noexcept {
  typed_constructor<value_t::number_integer>::construct(j,
                                                           static_cast<typename BasicJsonType::number_integer_t>(val));
}

template<typename BasicJsonType, typename EnumType,
    utils::enable_if_t<std::is_enum<EnumType>::value, int> = 0>
void to_json(BasicJsonType &j, EnumType e) noexcept {
  using underlying_type = typename std::underlying_type<EnumType>::type;
  typed_constructor<value_t::number_integer>::construct(j, static_cast<underlying_type>(e));
}

template<typename BasicJsonType>
void to_json(BasicJsonType &j, const std::vector<bool> &e) {
  typed_constructor<value_t::array>::construct(j, e);
}

template<
    typename BasicJsonType, typename CompatibleArrayType,
    utils::enable_if_t<
        utils::is_compatible_array_type<BasicJsonType, CompatibleArrayType>::value or
            std::is_same<typename BasicJsonType::array_t, CompatibleArrayType>::value,
        int> = 0>
void to_json(BasicJsonType &j, const CompatibleArrayType &arr) {
  typed_constructor<value_t::array>::construct(j, arr);
}

template<typename BasicJsonType>
void to_json(BasicJsonType &j, typename BasicJsonType::array_t &&arr) {
  typed_constructor<value_t::array>::construct(j, std::move(arr));
}

template<
    typename BasicJsonType, typename CompatibleObjectType,
    utils::enable_if_t<utils::is_compatible_object_type<BasicJsonType, CompatibleObjectType>::value,
        int> = 0>
void to_json(BasicJsonType &j, const CompatibleObjectType &obj) {
  typed_constructor<value_t::map>::construct(j, obj);
}

template<typename BasicJsonType>
void to_json(BasicJsonType &j, typename BasicJsonType::object_t &&obj) {
  typed_constructor<value_t::map>::construct(j, std::move(obj));
}

template<typename BasicJsonType, typename T, std::size_t N,
    utils::enable_if_t<not std::is_constructible<
        typename BasicJsonType::string_t, T (&)[N]>::value,
                int> = 0>
void to_json(BasicJsonType &j, T (&arr)[N]) {
  typed_constructor<value_t::array>::construct(j, arr);
}

template<typename BasicJsonType, typename... Args>
void to_json(BasicJsonType &j, const std::pair<Args...> &p) {
  j = {p.first, p.second};
}

template<typename BasicJsonType, typename Tuple, std::size_t... Idx>
void to_json_tuple_impl(BasicJsonType &j, const Tuple &t, utils::index_sequence<Idx...>) {
  j = {std::get<Idx>(t)...};
}

template<typename BasicJsonType, typename... Args>
void to_json(BasicJsonType &j, const std::tuple<Args...> &t) {
  to_json_tuple_impl(j, t, utils::index_sequence_for < Args... > {});
}


// overloads for basic_json template parameters
template<typename BasicJsonType, typename ArithmeticType,
    utils::enable_if_t<std::is_arithmetic<ArithmeticType>::value and
        not std::is_same<ArithmeticType,
                         typename BasicJsonType::boolean_t>::value,
                int> = 0>
void get_arithmetic_value(const BasicJsonType &j, ArithmeticType &val) {
  switch (static_cast<value_t>(j)) {
    case value_t::number_unsigned: {
      val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicJsonType::number_unsigned_t *>());
      break;
    }
    case value_t::number_integer: {
      val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicJsonType::number_integer_t *>());
      break;
    }
    case value_t::number_float: {
      val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicJsonType::number_float_t *>());
      break;
    }

    default:JSON_THROW(type_error::create(302, "type must be number, but is " + std::string(j.type_name())));
  }
}

template<typename BasicJsonType>
void from_json(const BasicJsonType &j, typename BasicJsonType::boolean_t &b) {
  if (JSON_UNLIKELY(not j.is_boolean())) {
    JSON_THROW(type_error::create(302, "type must be boolean, but is " + std::string(j.type_name())));
  }
  b = *j.template get_ptr<const typename BasicJsonType::boolean_t *>();
}

template<typename BasicJsonType>
void from_json(const BasicJsonType &j, typename BasicJsonType::string_t &s) {
  if (JSON_UNLIKELY(not j.is_string())) {
    JSON_THROW(type_error::create(302, "type must be string, but is " + std::string(j.type_name())));
  }
  s = *j.template get_ptr<const typename BasicJsonType::string_t *>();
}

template<typename BasicJsonType>
void from_json(const BasicJsonType &j, typename BasicJsonType::number_float_t &val) {
  get_arithmetic_value(j, val);
}

template<typename BasicJsonType>
void from_json(const BasicJsonType &j, typename BasicJsonType::number_unsigned_t &val) {
  get_arithmetic_value(j, val);
}

template<typename BasicJsonType>
void from_json(const BasicJsonType &j, typename BasicJsonType::number_integer_t &val) {
  get_arithmetic_value(j, val);
}

template<typename BasicJsonType, typename EnumType,
    utils::enable_if_t<std::is_enum<EnumType>::value, int> = 0>
void from_json(const BasicJsonType &j, EnumType &e) {
  typename std::underlying_type<EnumType>::type val;
  get_arithmetic_value(j, val);
  e = static_cast<EnumType>(val);
}

template<typename BasicJsonType>
void from_json(const BasicJsonType &j, typename BasicJsonType::array_t &arr) {
  if (JSON_UNLIKELY(not j.is_array())) {
    JSON_THROW(type_error::create(302, "type must be array, but is " + std::string(j.type_name())));
  }
  arr = *j.template get_ptr<const typename BasicJsonType::array_t *>();
}

// forward_list doesn't have an insert method
template<typename BasicJsonType, typename T, typename Allocator,
    utils::enable_if_t<std::is_convertible<BasicJsonType, T>::value, int> = 0>
void from_json(const BasicJsonType &j, std::forward_list <T, Allocator> &l) {
  if (JSON_UNLIKELY(not j.is_array())) {
    JSON_THROW(type_error::create(302, "type must be array, but is " + std::string(j.type_name())));
  }
  std::transform(j.rbegin(), j.rend(),
                 std::front_inserter(l), [](const BasicJsonType &i) {
        return i.template get<T>();
      });
}

template<typename BasicJsonType, typename CompatibleArrayType>
void from_json_array_impl(const BasicJsonType &j, CompatibleArrayType &arr, utils::priority_tag<0> /*unused*/) {
  using std::end;

  std::transform(j.begin(), j.end(),
                 std::inserter(arr, end(arr)), [](const BasicJsonType &i) {
        // get<BasicJsonType>() returns *this, this won't call a from_json
        // method when value_type is BasicJsonType
        return i.template get<typename CompatibleArrayType::value_type>();
      });
}

template<typename BasicJsonType, typename CompatibleArrayType>
auto from_json_array_impl(const BasicJsonType &j, CompatibleArrayType &arr, utils::priority_tag<1> /*unused*/)
-> decltype(
arr.reserve(std::declval<typename CompatibleArrayType::size_type>()),
    void()) {
  using std::end;

  arr.reserve(j.size());
  std::transform(j.begin(), j.end(),
                 std::inserter(arr, end(arr)), [](const BasicJsonType &i) {
        // get<BasicJsonType>() returns *this, this won't call a from_json
        // method when value_type is BasicJsonType
        return i.template get<typename CompatibleArrayType::value_type>();
      });
}

template<typename BasicJsonType, typename T, std::size_t N>
void from_json_array_impl(const BasicJsonType &j, std::array <T, N> &arr, utils::priority_tag<2> /*unused*/) {
  for (std::size_t i = 0; i < N; ++i) {
    arr[i] = j.at(i).template get<T>();
  }
}

template<typename BasicJsonType, typename CompatibleArrayType,
    utils::enable_if_t<utils::is_compatible_array_type<BasicJsonType, CompatibleArrayType>::value and
        std::is_convertible<BasicJsonType, typename CompatibleArrayType::value_type>::value and
        not std::is_same<typename BasicJsonType::array_t, CompatibleArrayType>::value, int> = 0>
void from_json(const BasicJsonType &j, CompatibleArrayType &arr) {
  if (JSON_UNLIKELY(not j.is_array())) {
    JSON_THROW(type_error::create(302, "type must be array, but is " + std::string(j.type_name())));
  }

  from_json_array_impl(j, arr, utils::priority_tag < 2 > {});
}

template<typename BasicJsonType, typename CompatibleObjectType,
    utils::enable_if_t<utils::is_compatible_object_type<BasicJsonType, CompatibleObjectType>::value, int> = 0>
void from_json(const BasicJsonType &j, CompatibleObjectType &obj) {
  if (JSON_UNLIKELY(not j.is_object())) {
    JSON_THROW(type_error::create(302, "type must be object, but is " + std::string(j.type_name())));
  }

  auto inner_object = j.template get_ptr<const typename BasicJsonType::object_t *>();
  using value_type = typename CompatibleObjectType::value_type;
  std::transform(
      inner_object->begin(), inner_object->end(),
      std::inserter(obj, obj.begin()),
      [](typename BasicJsonType::object_t::value_type const &p) {
        return value_type(p.first, p.second.template get<typename CompatibleObjectType::mapped_type>());
      });
}

// overload for arithmetic types, not chosen for basic_json template arguments
// (BooleanType, etc..); note: Is it really necessary to provide explicit
// overloads for boolean_t etc. in case of a custom BooleanType which is not
// an arithmetic type?
template<typename BasicJsonType, typename ArithmeticType,
    utils::enable_if_t<
        std::is_arithmetic<ArithmeticType>::value and
            not std::is_same<ArithmeticType, typename BasicJsonType::number_unsigned_t>::value and
            not std::is_same<ArithmeticType, typename BasicJsonType::number_integer_t>::value and
            not std::is_same<ArithmeticType, typename BasicJsonType::number_float_t>::value and
            not std::is_same<ArithmeticType, typename BasicJsonType::boolean_t>::value,
        int> = 0>
void from_json(const BasicJsonType &j, ArithmeticType &val) {
  switch (static_cast<value_t>(j)) {
    case value_t::number_unsigned: {
      val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicJsonType::number_unsigned_t *>());
      break;
    }
    case value_t::number_integer: {
      val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicJsonType::number_integer_t *>());
      break;
    }
    case value_t::number_float: {
      val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicJsonType::number_float_t *>());
      break;
    }
    case value_t::boolean: {
      val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicJsonType::boolean_t *>());
      break;
    }

    default:JSON_THROW(type_error::create(302, "type must be number, but is " + std::string(j.type_name())));
  }
}

template<typename BasicJsonType, typename... Args>
void from_json(const BasicJsonType &j, std::pair<Args...> &p) {
  p = {j.at(0), j.at(1)};
}

template<typename BasicJsonType, typename Tuple, std::size_t... Idx>
void from_json_tuple_impl(const BasicJsonType &j, Tuple &t, utils::index_sequence<Idx...>) {
  t = std::make_tuple(j.at(Idx)...);
}

template<typename BasicJsonType, typename... Args>
void from_json(const BasicJsonType &j, std::tuple<Args...> &t) {
  from_json_tuple_impl(j, t, utils::index_sequence_for < Args... > {});
}

struct to_json_fn {
 private:
  template<typename BasicJsonType, typename T>
  auto call(BasicJsonType &j, T &&val, utils::priority_tag<1> /*unused*/) const noexcept(noexcept(to_json(j,
                                                                                                   std::forward<T>(val))))
  -> decltype(to_json(j, std::forward<T>(val)), void()) {
    return to_json(j, std::forward<T>(val));
  }

  template<typename BasicJsonType, typename T>
  void call(BasicJsonType & /*unused*/, T && /*unused*/, utils::priority_tag<0> /*unused*/) const noexcept {
    static_assert(sizeof(BasicJsonType) == 0,
                  "could not find to_json() method in T's namespace");
  }

 public:
  template<typename BasicJsonType, typename T>
  void operator()(BasicJsonType &j, T &&val) const
  noexcept(noexcept(std::declval<to_json_fn>().call(j, std::forward<T>(val), utils::priority_tag < 1 > {}))) {
    return call(j, std::forward<T>(val), utils::priority_tag < 1 > {});
  }
};

struct from_json_fn {
 private:
  template<typename BasicJsonType, typename T>
  auto call(const BasicJsonType &j, T &val, utils::priority_tag<1> /*unused*/) const
  noexcept(noexcept(from_json(j, val)))
  -> decltype(from_json(j, val), void()) {
    return from_json(j, val);
  }

  template<typename BasicJsonType, typename T>
  void call(const BasicJsonType & /*unused*/, T & /*unused*/, utils::priority_tag<0> /*unused*/) const noexcept {
    static_assert(sizeof(BasicJsonType) == 0,
                  "could not find from_json() method in T's namespace");
  }

 public:
  template<typename BasicJsonType, typename T>
  void operator()(const BasicJsonType &j, T &val) const
  noexcept(noexcept(std::declval<from_json_fn>().call(j, val, utils::priority_tag < 1 > {}))) {
    return call(j, val, utils::priority_tag < 1 > {});
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
constexpr const auto &to_json = detail::static_const<detail::to_json_fn>::value;
constexpr const auto &from_json = detail::static_const<detail::from_json_fn>::value;
}

}
#endif //XCAPTCHA_SERIALIZER_H
