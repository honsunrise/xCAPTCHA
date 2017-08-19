//
// Created by zhsyourai on 8/19/17.
//

#ifndef XCAPTCHA_CONFIG_PATH_H
#define XCAPTCHA_CONFIG_PATH_H
#include <string>
#include "config/detail/declaration.h"

namespace captcha_config {

class config_path {
  XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
  friend
  class basic_config;

 public:
  explicit config_path(const std::string &s = "") : reference_tokens(split(s)) {}

  std::string to_string() const noexcept {
    return std::accumulate(reference_tokens.begin(), reference_tokens.end(),
                           std::string{},
                           [](const std::string &a, const std::string &b) {
                             return a + "/" + escape(b);
                           });
  }

  operator std::string() const {
    return to_string();
  }

 private:
  std::string pop_back() {
    if (JSON_UNLIKELY(is_root())) {
      JSON_THROW(detail::out_of_range::create(405, "JSON pointer has no parent"));
    }

    auto last = reference_tokens.back();
    reference_tokens.pop_back();
    return last;
  }

  bool is_root() const {
    return reference_tokens.empty();
  }

  config_path top() const {
    if (JSON_UNLIKELY(is_root())) {
      JSON_THROW(detail::out_of_range::create(405, "JSON pointer has no parent"));
    }

    config_path result = *this;
    result.reference_tokens = {reference_tokens[0]};
    return result;
  }

  XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
  XCAPTCHA_BASIC_CONFIG_TPL
  &
  get_and_create(XCAPTCHA_BASIC_CONFIG_TPL
                 &j) const;

  XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
  XCAPTCHA_BASIC_CONFIG_TPL
  &
  get_unchecked(XCAPTCHA_BASIC_CONFIG_TPL
                *ptr) const;

  XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
  XCAPTCHA_BASIC_CONFIG_TPL
  &
  get_checked(XCAPTCHA_BASIC_CONFIG_TPL
              *ptr) const;

  XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
  const XCAPTCHA_BASIC_CONFIG_TPL
  &
  get_unchecked(const XCAPTCHA_BASIC_CONFIG_TPL *ptr) const;

  XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
  const XCAPTCHA_BASIC_CONFIG_TPL
  &
  get_checked(const XCAPTCHA_BASIC_CONFIG_TPL *ptr) const;

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

  static void replace_substring(std::string &s, const std::string &f,
                                const std::string &t) {
    assert(not f.empty());
    for (auto pos = s.find(f);                // find first occurrence of f
         pos != std::string::npos;         // make sure f was found
         s.replace(pos, f.size(), t),      // replace with t, and
             pos = s.find(f, pos + t.size()))  // find next occurrence of f
    {}
  }

  static std::string escape(std::string s) {
    replace_substring(s, "~", "~0");
    replace_substring(s, "/", "~1");
    return s;
  }

  static void unescape(std::string &s) {
    replace_substring(s, "~1", "/");
    replace_substring(s, "~0", "~");
  }

  XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
  static void flatten(const std::string &reference_string,
                      const XCAPTCHA_BASIC_CONFIG_TPL &value,
                      XCAPTCHA_BASIC_CONFIG_TPL &result);

  XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
  static XCAPTCHA_BASIC_CONFIG_TPL
  unflatten(const XCAPTCHA_BASIC_CONFIG_TPL &value);

  friend bool operator==(config_path const &lhs,
                         config_path const &rhs) noexcept;

  friend bool operator!=(config_path const &lhs,
                         config_path const &rhs) noexcept;

  std::vector<std::string> reference_tokens;
};

}
#endif //XCAPTCHA_CONFIG_PATH_H
