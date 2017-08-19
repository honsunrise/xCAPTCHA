//
// Created by zhsyourai on 8/15/17.
//

#ifndef XCAPTCHA_IMPL_H
#define XCAPTCHA_IMPL_H
#include "config.h"
#include "declaration.h"
#include "config_path.h"

namespace captcha_config {
XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
XCAPTCHA_BASIC_CONFIG_TPL &
config_path::get_and_create(XCAPTCHA_BASIC_CONFIG_TPL &j) const {
  using size_type = typename XCAPTCHA_BASIC_CONFIG_TPL::size_type;
  auto result = &j;

// in case no reference tokens exist, return a reference to the JSON value
// j which will be overwritten by a primitive value
  for (const auto &reference_token : reference_tokens) {
    switch (result->m_type) {
      case detail::value_t::null: {
        if (reference_token == "0") {
          // start a new array if reference token is 0
          result = &result->operator[](0);
        } else {
          // start a new map otherwise
          result = &result->operator[](reference_token);
        }
        break;
      }

      case detail::value_t::map: {
        // create an entry in the map
        result = &result->operator[](reference_token);
        break;
      }

      case detail::value_t::array: {
        // create an entry in the array
        JSON_TRY
        {
          result = &result->operator[](static_cast<size_type>(std::stoi(reference_token)));
        }
        JSON_CATCH(std::invalid_argument & )
        {
          JSON_THROW(detail::parse_error::create(109, 0, "array index '" + reference_token + "' is not a number"));
        }
        break;
      }

/*
The following code is only reached if there exists a reference
token _and_ the current value is primitive. In this case, we have
an error situation, because primitive values may only occur as
single value; that is, with an empty list of reference tokens.
*/
      default:JSON_THROW(detail::type_error::create(313, "invalid value to unflatten"));
    }
  }

  return *result;
}

XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
XCAPTCHA_BASIC_CONFIG_TPL &
config_path::get_unchecked(XCAPTCHA_BASIC_CONFIG_TPL *ptr) const {
  using size_type = typename XCAPTCHA_BASIC_CONFIG_TPL::size_type;
  for (const auto &reference_token : reference_tokens) {
// convert null values to arrays or objects before continuing
    if (ptr->m_type == detail::value_t::null) {
// check if reference token is a number
      const bool nums =
          std::all_of(reference_token.begin(), reference_token.end(),
                      [](const char x) {
                        return (x >= '0' and x <= '9');
                      });

// change value to array for numbers or "-" or to map otherwise
      *ptr = (nums or reference_token == "-")
             ? detail::value_t::array
             : detail::value_t::map;
    }

    switch (ptr->m_type) {
      case detail::value_t::map: {
        // use unchecked map access
        ptr = &ptr->operator[](reference_token);
        break;
      }

      case detail::value_t::array: {
        // error condition (cf. RFC 6901, Sect. 4)
        if (JSON_UNLIKELY(reference_token.size() > 1 and reference_token[0] == '0')) {
          JSON_THROW(detail::parse_error::create(106, 0,
                                                 "array index '" + reference_token +
                                                     "' must not begin with '0'"));
        }

        if (reference_token == "-") {
          // explicitly treat "-" as index beyond the end
          ptr = &ptr->operator[](ptr->m_value.array->size());
        } else {
          // convert array index to number; unchecked access
          JSON_TRY
          {
            ptr = &ptr->operator[](
                static_cast<size_type>(std::stoi(reference_token)));
          }
          JSON_CATCH(std::invalid_argument & )
          {
            JSON_THROW(detail::parse_error::create(109, 0, "array index '" + reference_token + "' is not a number"));
          }
        }
        break;
      }

      default:JSON_THROW(detail::out_of_range::create(404, "unresolved reference token '" + reference_token + "'"));
    }
  }

  return *ptr;
}

XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
XCAPTCHA_BASIC_CONFIG_TPL &
config_path::get_checked(XCAPTCHA_BASIC_CONFIG_TPL *ptr) const {
  using size_type = typename XCAPTCHA_BASIC_CONFIG_TPL::size_type;
  for (const auto &reference_token : reference_tokens) {
    switch (ptr->m_type) {
      case detail::value_t::map: {
        // note: at performs range check
        ptr = &ptr->at(reference_token);
        break;
      }

      case detail::value_t::array: {
        if (JSON_UNLIKELY(reference_token == "-")) {
          // "-" always fails the range check
          JSON_THROW(detail::out_of_range::create(402,
                                                  "array index '-' (" + std::to_string(ptr->m_value.array->size()) +
                                                      ") is out of range"));
        }

        // error condition (cf. RFC 6901, Sect. 4)
        if (JSON_UNLIKELY(reference_token.size() > 1 and reference_token[0] == '0')) {
          JSON_THROW(detail::parse_error::create(106, 0,
                                                 "array index '" + reference_token +
                                                     "' must not begin with '0'"));
        }

        // note: at performs range check
        JSON_TRY
        {
          ptr = &ptr->at(static_cast<size_type>(std::stoi(reference_token)));
        }
        JSON_CATCH(std::invalid_argument & )
        {
          JSON_THROW(detail::parse_error::create(109, 0, "array index '" + reference_token + "' is not a number"));
        }
        break;
      }

      default:JSON_THROW(detail::out_of_range::create(404, "unresolved reference token '" + reference_token + "'"));
    }
  }

  return *ptr;
}

XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
const XCAPTCHA_BASIC_CONFIG_TPL &
config_path::get_unchecked(const XCAPTCHA_BASIC_CONFIG_TPL *ptr) const {
  using size_type = typename XCAPTCHA_BASIC_CONFIG_TPL::size_type;
  for (const auto &reference_token : reference_tokens) {
    switch (ptr->m_type) {
      case detail::value_t::map: {
        // use unchecked map access
        ptr = &ptr->operator[](reference_token);
        break;
      }

      case detail::value_t::array: {
        if (JSON_UNLIKELY(reference_token == "-")) {
          // "-" cannot be used for const access
          JSON_THROW(detail::out_of_range::create(402,
                                                  "array index '-' (" + std::to_string(ptr->m_value.array->size()) +
                                                      ") is out of range"));
        }

        // error condition (cf. RFC 6901, Sect. 4)
        if (JSON_UNLIKELY(reference_token.size() > 1 and reference_token[0] == '0')) {
          JSON_THROW(detail::parse_error::create(106, 0,
                                                 "array index '" + reference_token +
                                                     "' must not begin with '0'"));
        }

        // use unchecked array access
        JSON_TRY
        {
          ptr = &ptr->operator[](
              static_cast<size_type>(std::stoi(reference_token)));
        }
        JSON_CATCH(std::invalid_argument & )
        {
          JSON_THROW(detail::parse_error::create(109, 0, "array index '" + reference_token + "' is not a number"));
        }
        break;
      }

      default:JSON_THROW(detail::out_of_range::create(404, "unresolved reference token '" + reference_token + "'"));
    }
  }

  return *ptr;
}

XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
const XCAPTCHA_BASIC_CONFIG_TPL &
config_path::get_checked(const XCAPTCHA_BASIC_CONFIG_TPL *ptr) const {
  using size_type = typename XCAPTCHA_BASIC_CONFIG_TPL::size_type;
  for (const auto &reference_token : reference_tokens) {
    switch (ptr->m_type) {
      case detail::value_t::map: {
        // note: at performs range check
        ptr = &ptr->at(reference_token);
        break;
      }

      case detail::value_t::array: {
        if (JSON_UNLIKELY(reference_token == "-")) {
          // "-" always fails the range check
          JSON_THROW(detail::out_of_range::create(402,
                                                  "array index '-' (" + std::to_string(ptr->m_value.array->size()) +
                                                      ") is out of range"));
        }

        // error condition (cf. RFC 6901, Sect. 4)
        if (JSON_UNLIKELY(reference_token.size() > 1 and reference_token[0] == '0')) {
          JSON_THROW(detail::parse_error::create(106, 0,
                                                 "array index '" + reference_token +
                                                     "' must not begin with '0'"));
        }

        // note: at performs range check
        JSON_TRY
        {
          ptr = &ptr->at(static_cast<size_type>(std::stoi(reference_token)));
        }
        JSON_CATCH(std::invalid_argument & )
        {
          JSON_THROW(detail::parse_error::create(109, 0, "array index '" + reference_token + "' is not a number"));
        }
        break;
      }

      default:JSON_THROW(detail::out_of_range::create(404, "unresolved reference token '" + reference_token + "'"));
    }
  }

  return *ptr;
}

XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
void config_path::flatten(const std::string &reference_string,
                           const XCAPTCHA_BASIC_CONFIG_TPL &value,
                           XCAPTCHA_BASIC_CONFIG_TPL &result) {
  switch (value.m_type) {
    case detail::value_t::array: {
      if (value.m_value.array->empty()) {
        // flatten empty array as null
        result[reference_string] = nullptr;
      } else {
        // iterate array and use index as reference string
        for (std::size_t i = 0; i < value.m_value.array->size(); ++i) {
          flatten(reference_string + "/" + std::to_string(i),
                  value.m_value.array->operator[](i), result);
        }
      }
      break;
    }

    case detail::value_t::map: {
      if (value.m_value.object->empty()) {
        // flatten empty map as null
        result[reference_string] = nullptr;
      } else {
        // iterate map and use keys as reference string
        for (const auto &element : *value.m_value.object) {
          flatten(reference_string + "/" + escape(element.first), element.second, result);
        }
      }
      break;
    }

    default: {
      // add primitive value with its reference string
      result[reference_string] = value;
      break;
    }
  }
}

XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
XCAPTCHA_BASIC_CONFIG_TPL
config_path::unflatten(const XCAPTCHA_BASIC_CONFIG_TPL &value) {
  if (JSON_UNLIKELY(not value.is_object())) {
    JSON_THROW(detail::type_error::create(314, "only objects can be unflattened"));
  }

  XCAPTCHA_BASIC_CONFIG_TPL result;

  // iterate the JSON map values
  for (const auto &element : *value.m_value.object) {
    if (JSON_UNLIKELY(not element.second.is_primitive())) {
      JSON_THROW(detail::type_error::create(315, "values in map must be primitive"));
    }

    // assign value to reference pointed to by JSON pointer; Note that if
    // the JSON pointer is "" (i.e., points to the whole value), function
    // get_and_create returns a reference to result itself. An assignment
    // will then create a primitive value.
    config_path(element.first).get_and_create(result) = element.second;
  }

  return result;
}

inline bool operator==(config_path const &lhs, config_path const &rhs) noexcept {
  return (lhs.reference_tokens == rhs.reference_tokens);
}

inline bool operator!=(config_path const &lhs, config_path const &rhs) noexcept {
  return not(lhs == rhs);
}
} // namespace captcha_config
#endif //XCAPTCHA_IMPL_H
