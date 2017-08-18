//
// Created by zhsyourai on 8/19/17.
//

#ifndef XCAPTCHA_CONFIG_PATH_H
#define XCAPTCHA_CONFIG_PATH_H
#include <string>
#include "declaration.h"

namespace captcha_config {

class json_pointer {
  /// allow basic_json to access private members
  XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
  friend
  class basic_config;

 public:
  /*!
  @brief create JSON pointer

  Create a JSON pointer according to the syntax described in
  [Section 3 of RFC6901](https://tools.ietf.org/html/rfc6901#section-3).

  @param[in] s  string representing the JSON pointer; if omitted, the empty
                string is assumed which references the whole JSON value

  @throw parse_error.107 if the given JSON pointer @a s is nonempty and
  does not begin with a slash (`/`); see example below

  @throw parse_error.108 if a tilde (`~`) in the given JSON pointer @a s
  is not followed by `0` (representing `~`) or `1` (representing `/`);
  see example below

  @liveexample{The example shows the construction several valid JSON
  pointers as well as the exceptional behavior.,json_pointer}

  @since version 2.0.0
  */
  explicit json_pointer(const std::string &s = "") : reference_tokens(split(s)) {}

  /*!
  @brief return a string representation of the JSON pointer

  @invariant For each JSON pointer `ptr`, it holds:
  @code {.cpp}
  ptr == json_pointer(ptr.to_string());
  @endcode

  @return a string representation of the JSON pointer

  @liveexample{The example shows the result of `to_string`.,
  json_pointer__to_string}

  @since version 2.0.0
  */
  std::string to_string() const noexcept {
    return std::accumulate(reference_tokens.begin(), reference_tokens.end(),
                           std::string{},
                           [](const std::string &a, const std::string &b) {
                             return a + "/" + escape(b);
                           });
  }

  /// @copydoc to_string()
  operator std::string() const {
    return to_string();
  }

 private:
  /*!
  @brief remove and return last reference pointer
  @throw out_of_range.405 if JSON pointer has no parent
  */
  std::string pop_back() {
    if (JSON_UNLIKELY(is_root())) {
      JSON_THROW(detail::out_of_range::create(405, "JSON pointer has no parent"));
    }

    auto last = reference_tokens.back();
    reference_tokens.pop_back();
    return last;
  }

  /// return whether pointer points to the root document
  bool is_root() const {
    return reference_tokens.empty();
  }

  json_pointer top() const {
    if (JSON_UNLIKELY(is_root())) {
      JSON_THROW(detail::out_of_range::create(405, "JSON pointer has no parent"));
    }

    json_pointer result = *this;
    result.reference_tokens = {reference_tokens[0]};
    return result;
  }

  /*!
  @brief create and return a reference to the pointed to value

  @complexity Linear in the number of reference tokens.

  @throw parse_error.109 if array index is not a number
  @throw type_error.313 if value cannot be unflattened
  */
  XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
  XCAPTCHA_BASIC_CONFIG_TPL
  &
  get_and_create(XCAPTCHA_BASIC_CONFIG_TPL
                 &j) const;

  /*!
  @brief return a reference to the pointed to value

  @note This version does not throw if a value is not present, but tries to
        create nested values instead. For instance, calling this function
        with pointer `"/this/that"` on a null value is equivalent to calling
        `operator[]("this").operator[]("that")` on that value, effectively
        changing the null value to an object.

  @param[in] ptr  a JSON value

  @return reference to the JSON value pointed to by the JSON pointer

  @complexity Linear in the length of the JSON pointer.

  @throw parse_error.106   if an array index begins with '0'
  @throw parse_error.109   if an array index was not a number
  @throw out_of_range.404  if the JSON pointer can not be resolved
  */
  XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
  XCAPTCHA_BASIC_CONFIG_TPL
  &
  get_unchecked(XCAPTCHA_BASIC_CONFIG_TPL
                *ptr) const;

  /*!
  @throw parse_error.106   if an array index begins with '0'
  @throw parse_error.109   if an array index was not a number
  @throw out_of_range.402  if the array index '-' is used
  @throw out_of_range.404  if the JSON pointer can not be resolved
  */
  XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
  XCAPTCHA_BASIC_CONFIG_TPL
  &
  get_checked(XCAPTCHA_BASIC_CONFIG_TPL
              *ptr) const;

  /*!
  @brief return a const reference to the pointed to value

  @param[in] ptr  a JSON value

  @return const reference to the JSON value pointed to by the JSON
  pointer

  @throw parse_error.106   if an array index begins with '0'
  @throw parse_error.109   if an array index was not a number
  @throw out_of_range.402  if the array index '-' is used
  @throw out_of_range.404  if the JSON pointer can not be resolved
  */
  XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
  const XCAPTCHA_BASIC_CONFIG_TPL
  &
  get_unchecked(const XCAPTCHA_BASIC_CONFIG_TPL *ptr) const;

  /*!
  @throw parse_error.106   if an array index begins with '0'
  @throw parse_error.109   if an array index was not a number
  @throw out_of_range.402  if the array index '-' is used
  @throw out_of_range.404  if the JSON pointer can not be resolved
  */
  XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
  const XCAPTCHA_BASIC_CONFIG_TPL
  &
  get_checked(const XCAPTCHA_BASIC_CONFIG_TPL *ptr) const;

  /*!
  @brief split the string input to reference tokens

  @note This function is only called by the json_pointer constructor.
        All exceptions below are documented there.

  @throw parse_error.107  if the pointer is not empty or begins with '/'
  @throw parse_error.108  if character '~' is not followed by '0' or '1'
  */
  static std::vector<std::string> split(const std::string &reference_string) {
    std::vector<std::string> result;

    // special case: empty reference string -> no reference tokens
    if (reference_string.empty()) {
      return result;
    }

    // check if nonempty reference string begins with slash
    if (JSON_UNLIKELY(reference_string[0] != '/')) {
      JSON_THROW(detail::parse_error::create(107, 1,
                                             "JSON pointer must be empty or begin with '/' - was: '" +
                                                 reference_string + "'"));
    }

    // extract the reference tokens:
    // - slash: position of the last read slash (or end of string)
    // - start: position after the previous slash
    for (
      // search for the first slash after the first character
        std::size_t slash = reference_string.find_first_of('/', 1),
        // set the beginning of the first reference token
            start = 1;
      // we can stop if start == string::npos+1 = 0
        start != 0;
      // set the beginning of the next reference token
      // (will eventually be 0 if slash == std::string::npos)
        start = slash + 1,
            // find next slash
            slash = reference_string.find_first_of('/', start)) {
      // use the text between the beginning of the reference token
      // (start) and the last slash (slash).
      auto reference_token = reference_string.substr(start, slash - start);

      // check reference tokens are properly escaped
      for (std::size_t pos = reference_token.find_first_of('~');
           pos != std::string::npos;
           pos = reference_token.find_first_of('~', pos + 1)) {
        assert(reference_token[pos] == '~');

        // ~ must be followed by 0 or 1
        if (JSON_UNLIKELY(pos == reference_token.size() - 1 or
            (reference_token[pos + 1] != '0' and
                reference_token[pos + 1] != '1'))) {
          JSON_THROW(detail::parse_error::create(108, 0, "escape character '~' must be followed with '0' or '1'"));
        }
      }

      // finally, store the reference token
      unescape(reference_token);
      result.push_back(reference_token);
    }

    return result;
  }

  /*!
  @brief replace all occurrences of a substring by another string

  @param[in,out] s  the string to manipulate; changed so that all
                 occurrences of @a f are replaced with @a t
  @param[in]     f  the substring to replace with @a t
  @param[in]     t  the string to replace @a f

  @pre The search string @a f must not be empty. **This precondition is
  enforced with an assertion.**

  @since version 2.0.0
  */
  static void replace_substring(std::string &s, const std::string &f,
                                const std::string &t) {
    assert(not f.empty());
    for (auto pos = s.find(f);                // find first occurrence of f
         pos != std::string::npos;         // make sure f was found
         s.replace(pos, f.size(), t),      // replace with t, and
             pos = s.find(f, pos + t.size()))  // find next occurrence of f
    {}
  }

  /// escape "~"" to "~0" and "/" to "~1"
  static std::string escape(std::string s) {
    replace_substring(s, "~", "~0");
    replace_substring(s, "/", "~1");
    return s;
  }

  /// unescape "~1" to tilde and "~0" to slash (order is important!)
  static void unescape(std::string &s) {
    replace_substring(s, "~1", "/");
    replace_substring(s, "~0", "~");
  }

  /*!
  @param[in] reference_string  the reference string to the current value
  @param[in] value             the value to consider
  @param[in,out] result        the result object to insert values to

  @note Empty objects or arrays are flattened to `null`.
  */
  XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
  static void flatten(const std::string &reference_string,
                      const XCAPTCHA_BASIC_CONFIG_TPL &value,
                      XCAPTCHA_BASIC_CONFIG_TPL &result);

  /*!
  @param[in] value  flattened JSON

  @return unflattened JSON

  @throw parse_error.109 if array index is not a number
  @throw type_error.314  if value is not an object
  @throw type_error.315  if object values are not primitive
  @throw type_error.313  if value cannot be unflattened
  */
  XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
  static XCAPTCHA_BASIC_CONFIG_TPL
  unflatten(const XCAPTCHA_BASIC_CONFIG_TPL &value);

  friend bool operator==(json_pointer const &lhs,
                         json_pointer const &rhs) noexcept;

  friend bool operator!=(json_pointer const &lhs,
                         json_pointer const &rhs) noexcept;

  /// the reference tokens
  std::vector<std::string> reference_tokens;
};

}
#endif //XCAPTCHA_CONFIG_PATH_H
