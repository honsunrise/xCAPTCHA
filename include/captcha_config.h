//
// Created by zhsyourai on 8/7/17.
//

#ifndef XCAPTCHA_CAPTCHA_CONFIG_H
#define XCAPTCHA_CAPTCHA_CONFIG_H
#include <string>
#include <vector>
#include <map>
namespace captcha_config {

enum _TYPE {
  TYPE_INT8,
  TYPE_INT16,
  TYPE_INT32,
  TYPE_STRING,
  TYPE_NODE,
  TYPE_NONE,
};

enum _RULE_TYPE {
  RULE_MIN_MAX,
  RULE_REGEX,
  RULE_ENUM,
  RULE_NODE,
  RULE_NONE
};

union rule{
  struct {
    int64_t min;
    int64_t max;
  };
  std::string regex;
  std::vector<std::string> enumerate;
};

class config_define {
 public:
  config_define();

  void add(std::string key, const std::string &regex);
  void add(std::string key, int64_t min, int64_t max);
  void add(std::string key, const std::vector<std::string> &list);
  void add(std::string key, const config_define &value);
  void add(std::string key, config_define &&value);

  config_define &operator[](std::string key);

  std::tuple<int64_t, int64_t> min_max();
  std::string regex();
 private:
  _RULE_TYPE type;
  union {
    rule _rule;
    std::map<std::string, config_define> _defines;
  };
};

class node {
 public:
  node();
  node &operator=(node &&other) noexcept;
  explicit operator int8_t();
  explicit operator int16_t();
  explicit operator int32_t();
  explicit operator std::string();

  void add(std::string key, const std::string &value);
  void add(std::string key, std::string &&value);
  void add(std::string key, int8_t value);
  void add(std::string key, int16_t value);
  void add(std::string key, int32_t value);
  void add(std::string key, const node &value);
  void add(std::string key, node &&value);

  std::string node::key();
  const node &node::value();


 private:
  _TYPE _type;
  union {
    struct {
      std::string _key;
      union {
        int8_t _i8_v;
        int16_t _i16_v;
        int32_t _i32_v;
        std::string _s_v;
      };
    };
    std::map<std::string, node> _node;
  };
};
}

#endif //XCAPTCHA_CAPTCHA_CONFIG_H
