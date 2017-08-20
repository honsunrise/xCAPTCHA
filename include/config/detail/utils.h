//
// Created by zhsyourai on 8/15/17.
//

#ifndef XCAPTCHA_UTILS_H
#define XCAPTCHA_UTILS_H
#include <type_traits>
#include "declaration.h"
// allow to disable exceptions
#if (defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)) && not defined(JSON_NOEXCEPTION)
  #define JSON_THROW(exception) throw exception
  #define JSON_TRY try
  #define JSON_CATCH(exception) catch(exception)
#else
  #define JSON_THROW(exception) std::abort()
  #define JSON_TRY if(true)
  #define JSON_CATCH(exception) if(false)
#endif

// manual branch prediction
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
  #define JSON_LIKELY(x)      __builtin_expect(!!(x), 1)
  #define JSON_UNLIKELY(x)    __builtin_expect(!!(x), 0)
#else
  #define JSON_LIKELY(x)      x
  #define JSON_UNLIKELY(x)    x
#endif

namespace captcha_config {
namespace utils {

template<typename>
struct is_basic_config : std::false_type {};

XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
struct is_basic_config<XCAPTCHA_BASIC_CONFIG_TPL> : std::true_type {};

XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
struct is_basic_config<XCAPTCHA_BASIC_CONFIG_DEFINE_TPL> : std::true_type {};

template<bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template<typename T>
using uncvref_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

// implementation of C++14 index_sequence and affiliates
// source: https://stackoverflow.com/a/32223343
template<std::size_t... Ints>
struct index_sequence {
  using type = index_sequence;
  using value_type = std::size_t;
  static constexpr std::size_t size() noexcept {
    return sizeof...(Ints);
  }
};

template<class Sequence1, class Sequence2>
struct merge_and_renumber;

template<std::size_t... I1, std::size_t... I2>
struct merge_and_renumber<index_sequence<I1...>, index_sequence<I2...>>
    : index_sequence<I1..., (sizeof...(I1) + I2)...> {
};

template<std::size_t N>
struct make_index_sequence
    : merge_and_renumber<typename make_index_sequence<N / 2>::type,
                         typename make_index_sequence<N - N / 2>::type> {
};

template<>
struct make_index_sequence<0> : index_sequence<> {};
template<>
struct make_index_sequence<1> : index_sequence<0> {};

template<typename... Ts>
using index_sequence_for = make_index_sequence<sizeof...(Ts)>;

/*
Implementation of two C++17 constructs: conjunction, negation. This is needed
to avoid evaluating all the traits in a condition

For example: not std::is_same<void, T>::value and has_value_type<T>::value
will not compile when T = void (on MSVC at least). Whereas
conjunction<negation<std::is_same<void, T>>, has_value_type<T>>::value will
stop evaluating if negation<...>::value == false

Please note that those constructs must be used with caution, since symbols can
become very long quickly (which can slow down compilation and cause MSVC
internal compiler errors). Only use it when you have to (see example ahead).
*/
template<class...>
struct conjunction : std::true_type {};
template<class B1>
struct conjunction<B1> : B1 {};
template<class B1, class... Bn>
struct conjunction<B1, Bn...> : std::conditional<bool(B1::value), conjunction<Bn...>, B1>::type {};

template<class B>
struct negation : std::integral_constant<bool, !B::value> {};

// dispatch utility (taken from ranges-v3)
template<unsigned N>
struct priority_tag : priority_tag<N - 1> {};
template<>
struct priority_tag<0> {};

////////////////////////
// has_/is_ functions //
////////////////////////

/*!
@brief Helper to determine whether there's a key_type for T.

This helper is used to tell associative containers apart from other containers
such as sequence containers. For instance, `std::map` passes the test as it
contains a `mapped_type`, whereas `std::vector` fails the test.

@sa http://stackoverflow.com/a/7728728/266378
@since version 1.0.0, overworked in version 2.0.6
*/
#define NLOHMANN_JSON_HAS_HELPER(type)                                        \
    template<typename T> struct has_##type {                                  \
    private:                                                                  \
        template<typename U, typename = typename U::type>                     \
        static int detect(U &&);                                              \
        static void detect(...);                                              \
    public:                                                                   \
        static constexpr bool value =                                         \
                std::is_integral<decltype(detect(std::declval<T>()))>::value; \
    }

NLOHMANN_JSON_HAS_HELPER(mapped_type);
NLOHMANN_JSON_HAS_HELPER(key_type);
NLOHMANN_JSON_HAS_HELPER(value_type);
NLOHMANN_JSON_HAS_HELPER(iterator);

#undef NLOHMANN_JSON_HAS_HELPER


template<bool B, class RealType, class CompatibleObjectType>
struct is_compatible_object_type_impl : std::false_type {};

template<class RealType, class CompatibleObjectType>
struct is_compatible_object_type_impl<true, RealType, CompatibleObjectType>
{
  static constexpr auto value =
      std::is_constructible<typename RealType::key_type, typename CompatibleObjectType::key_type>::value and
          std::is_constructible<typename RealType::mapped_type, typename CompatibleObjectType::mapped_type>::value;
};

template<class BasicConfigType, class CompatibleObjectType>
struct is_compatible_map_type
{
  static auto constexpr value = is_compatible_object_type_impl <
      conjunction<negation<std::is_same<void, CompatibleObjectType>>,
                  has_mapped_type<CompatibleObjectType>,
                  has_key_type<CompatibleObjectType>>::value,
      typename BasicConfigType::object_t, CompatibleObjectType >::value;
};

template<typename BasicConfigType, typename T>
struct is_basic_config_nested_type
{
  static auto constexpr value =
      std::is_same<T, typename BasicConfigType::iterator>::value or
      std::is_same<T, typename BasicConfigType::const_iterator>::value or
      std::is_same<T, typename BasicConfigType::reverse_iterator>::value or
      std::is_same<T, typename BasicConfigType::const_reverse_iterator>::value;
};

template<class BasicConfigType, class CompatibleArrayType>
struct is_compatible_array_type
{
  static auto constexpr value =
      conjunction<negation<std::is_same<void, CompatibleArrayType>>,
                  negation<is_compatible_map_type<
                      BasicConfigType, CompatibleArrayType>>,
                  negation<std::is_constructible<typename BasicConfigType::string_t,
                                                 CompatibleArrayType>>,
                  negation<is_basic_config_nested_type<BasicConfigType, CompatibleArrayType>>,
                  has_value_type<CompatibleArrayType>,
                  has_iterator<CompatibleArrayType>>::value;
};

template<bool, typename, typename>
struct is_compatible_integer_type_impl : std::false_type {};

template<typename RealIntegerType, typename CompatibleNumberIntegerType>
struct is_compatible_integer_type_impl<true, RealIntegerType, CompatibleNumberIntegerType>
{
  // is there an assert somewhere on overflows?
  using RealLimits = std::numeric_limits<RealIntegerType>;
  using CompatibleLimits = std::numeric_limits<CompatibleNumberIntegerType>;

  static constexpr auto value =
      std::is_constructible<RealIntegerType, CompatibleNumberIntegerType>::value and
          CompatibleLimits::is_integer and
          RealLimits::is_signed == CompatibleLimits::is_signed;
};

template<typename RealIntegerType, typename CompatibleNumberIntegerType>
struct is_compatible_integer_type
{
  static constexpr auto value =
      is_compatible_integer_type_impl <
          std::is_integral<CompatibleNumberIntegerType>::value and
              not std::is_same<bool, CompatibleNumberIntegerType>::value,
          RealIntegerType, CompatibleNumberIntegerType > ::value;
};


// trait checking if Serializer<T>::from_json(json const&, udt&) exists
template<typename BasicConfigType, typename T>
struct has_from_config
{
 private:
  // also check the return type of from_json
  template<typename U, typename = enable_if_t<std::is_same<void, decltype(uncvref_t<U>::from_json(
      std::declval<BasicConfigType>(), std::declval<T&>()))>::value>>
  static int detect(U&&);
  static void detect(...);

 public:
  static constexpr bool value = std::is_integral<decltype(
                                                 detect(std::declval<typename BasicConfigType::template serializer<T, void>>()))>::value;
};

// This trait checks if Serializer<T>::from_json(json const&) exists
// this overload is used for non-default-constructible user-defined-types
template<typename BasicConfigType, typename T>
struct has_non_default_from_config
{
 private:
  template <
      typename U,
      typename = enable_if_t<std::is_same<
          T, decltype(uncvref_t<U>::from_json(std::declval<BasicConfigType>()))>::value >>
  static int detect(U&&);
  static void detect(...);

 public:
  static constexpr bool value = std::is_integral<decltype(detect(
      std::declval<typename BasicConfigType::template erializer<T, void>>()))>::value;
};

// This trait checks if BasicConfigType::serializer<T>::to_json exists
template<typename BasicConfigType, typename T>
struct has_to_config
{
 private:
  template<typename U, typename = decltype(uncvref_t<U>::to_json(
      std::declval<BasicConfigType&>(), std::declval<T>()))>
  static int detect(U&&);
  static void detect(...);

 public:
  static constexpr bool value = std::is_integral<decltype(detect(
      std::declval<typename BasicConfigType::template serializer<T, void>>()))>::value;
};

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
}
#endif //XCAPTCHA_UTILS_H
