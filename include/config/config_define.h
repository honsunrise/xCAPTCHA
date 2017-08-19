//
// Created by zhsyourai on 8/19/17.
//

#ifndef XCAPTCHA_CONFIG_DEFINE_H
#define XCAPTCHA_CONFIG_DEFINE_H
#include "config/detail/basic.h"
namespace captcha_config {

XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
class config_define : public XCAPTCHA_BASIC_CONFIG_TPL {

};

using config_define = config_define<>;

}
#endif //XCAPTCHA_CONFIG_DEFINE_H
