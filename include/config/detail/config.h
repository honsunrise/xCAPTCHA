//
// Created by zhsyourai on 8/15/17.
//

#ifndef XCAPTCHA_DETAIL_CONFIG_H
#define XCAPTCHA_DETAIL_CONFIG_H
#include <boost/noncopyable.hpp>
#include <boost/any.hpp>
#include "iterator.h"
#include "config/rules.h"

namespace captcha_config {
namespace detail {
enum class value_t : uint8_t
{
  null,             ///< null value
  object,           ///< object (unordered set of name/value pairs)
  array,            ///< array (ordered collection of values)
  string,           ///< string value
  boolean,          ///< boolean value
  number_integer,   ///< number value (signed integer)
  number_unsigned,  ///< number value (unsigned integer)
  number_float,     ///< number value (floating-point)
  discarded         ///< discarded by the the parser callback function
};

template<typename BasicConfigType>
class json_ref
{
 public:
  using value_type = BasicConfigType;

  json_ref(value_type&& value)
      : owned_value(std::move(value)),
        value_ref(&owned_value),
        is_rvalue(true)
  {}

  json_ref(const value_type& value)
      : value_ref(const_cast<value_type*>(&value)),
        is_rvalue(false)
  {}

  json_ref(std::initializer_list<json_ref> init)
      : owned_value(init),
        value_ref(&owned_value),
        is_rvalue(true)
  {}

  template <class... Args>
  json_ref(Args... args)
      : owned_value(std::forward<Args>(args)...),
        value_ref(&owned_value),
        is_rvalue(true)
  {}

  // class should be movable only
  json_ref(json_ref&&) = default;
  json_ref(const json_ref&) = delete;
  json_ref& operator=(const json_ref&) = delete;

  value_type moved_or_copied() const
  {
    if (is_rvalue)
    {
      return std::move(*value_ref);
    }
    else
    {
      return *value_ref;
    }
  }

  value_type const& operator*() const
  {
    return *static_cast<value_type const*>(value_ref);
  }

  value_type const* operator->() const
  {
    return static_cast<value_type const*>(value_ref);
  }

 private:
  mutable value_type owned_value = nullptr;
  value_type* value_ref = nullptr;
  const bool is_rvalue;
};
}
}
#endif //XCAPTCHA_DETAIL_CONFIG_H
