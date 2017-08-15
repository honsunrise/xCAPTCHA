//
// Created by zhsyourai on 8/15/17.
//

#ifndef XCAPTCHA_UTILS_H
#define XCAPTCHA_UTILS_H
#include <type_traits>
namespace captcha_config {
namespace utils {

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
