//
// Created by zhsyourai on 8/19/17.
//

#ifndef XCAPTCHA_VALUE_TYPE_H
#define XCAPTCHA_VALUE_TYPE_H

#include <cstdint>

namespace captcha_config {
namespace detail {

enum class value_t : uint8_t {
  null,             ///< null value
  map,           ///< object (unordered set of name/value pairs)
  array,            ///< array (ordered collection of values)
  string,           ///< string value
  boolean,          ///< boolean value
  number_integer,   ///< number value (signed integer)
  number_unsigned,  ///< number value (unsigned integer)
  number_float,     ///< number value (floating-point)
  discarded         ///< discarded by the the parser callback function
};

}
}
#endif //XCAPTCHA_VALUE_TYPE_H
