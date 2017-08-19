//
// Created by zhsyourai on 8/19/17.
//

#ifndef XCAPTCHA_TYPED_CONSTRUCTOR_H
#define XCAPTCHA_TYPED_CONSTRUCTOR_H
#include "value_type.h"
#include "utils.h"

namespace captcha_config {
namespace detail {


template<value_t> struct typed_constructor;

template<>
struct typed_constructor<value_t::boolean>
{
  template<typename BasicJsonType>
  static void construct(BasicJsonType& j, typename BasicJsonType::boolean_t b) noexcept
  {
    j.m_type = value_t::boolean;
    j.m_value = b;
    j.assert_invariant();
  }
};

template<>
struct typed_constructor<value_t::string>
{
  template<typename BasicJsonType>
  static void construct(BasicJsonType& j, const typename BasicJsonType::string_t& s)
  {
    j.m_type = value_t::string;
    j.m_value = s;
    j.assert_invariant();
  }

  template<typename BasicJsonType>
  static void construct(BasicJsonType& j, typename BasicJsonType::string_t&& s)
  {
    j.m_type = value_t::string;
    j.m_value = std::move(s);
    j.assert_invariant();
  }
};

template<>
struct typed_constructor<value_t::number_float>
{
  template<typename BasicJsonType>
  static void construct(BasicJsonType& j, typename BasicJsonType::number_float_t val) noexcept
  {
    j.m_type = value_t::number_float;
    j.m_value = val;
    j.assert_invariant();
  }
};

template<>
struct typed_constructor<value_t::number_unsigned>
{
  template<typename BasicJsonType>
  static void construct(BasicJsonType& j, typename BasicJsonType::number_unsigned_t val) noexcept
  {
    j.m_type = value_t::number_unsigned;
    j.m_value = val;
    j.assert_invariant();
  }
};

template<>
struct typed_constructor<value_t::number_integer>
{
  template<typename BasicJsonType>
  static void construct(BasicJsonType& j, typename BasicJsonType::number_integer_t val) noexcept
  {
    j.m_type = value_t::number_integer;
    j.m_value = val;
    j.assert_invariant();
  }
};

template<>
struct typed_constructor<value_t::array>
{
  template<typename BasicJsonType>
  static void construct(BasicJsonType& j, const typename BasicJsonType::array_t& arr)
  {
    j.m_type = value_t::array;
    j.m_value = arr;
    j.assert_invariant();
  }

  template<typename BasicJsonType>
  static void construct(BasicJsonType& j, typename BasicJsonType::array_t&& arr)
  {
    j.m_type = value_t::array;
    j.m_value = std::move(arr);
    j.assert_invariant();
  }

  template<typename BasicJsonType, typename CompatibleArrayType,
      utils::enable_if_t<not std::is_same<CompatibleArrayType,
                                   typename BasicJsonType::array_t>::value,
                  int> = 0>
  static void construct(BasicJsonType& j, const CompatibleArrayType& arr)
  {
    using std::begin;
    using std::end;
    j.m_type = value_t::array;
    j.m_value.array = j.template create<typename BasicJsonType::array_t>(begin(arr), end(arr));
    j.assert_invariant();
  }

  template<typename BasicJsonType>
  static void construct(BasicJsonType& j, const std::vector<bool>& arr)
  {
    j.m_type = value_t::array;
    j.m_value = value_t::array;
    j.m_value.array->reserve(arr.size());
    for (bool x : arr)
    {
      j.m_value.array->push_back(x);
    }
    j.assert_invariant();
  }
};

template<>
struct typed_constructor<value_t::map>
{
  template<typename BasicJsonType>
  static void construct(BasicJsonType& j, const typename BasicJsonType::object_t& obj)
  {
    j.m_type = value_t::map;
    j.m_value = obj;
    j.assert_invariant();
  }

  template<typename BasicJsonType>
  static void construct(BasicJsonType& j, typename BasicJsonType::object_t&& obj)
  {
    j.m_type = value_t::map;
    j.m_value = std::move(obj);
    j.assert_invariant();
  }

  template<typename BasicJsonType, typename CompatibleObjectType,
      utils::enable_if_t<not std::is_same<CompatibleObjectType,
                                   typename BasicJsonType::object_t>::value, int> = 0>
  static void construct(BasicJsonType& j, const CompatibleObjectType& obj)
  {
    using std::begin;
    using std::end;

    j.m_type = value_t::map;
    j.m_value.object = j.template create<typename BasicJsonType::object_t>(begin(obj), end(obj));
    j.assert_invariant();
  }
};

}
}

#endif //XCAPTCHA_TYPED_CONSTRUCTOR_H
