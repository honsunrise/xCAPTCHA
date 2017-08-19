//
// Created by zhsyourai on 8/7/17.
//

#ifndef XCAPTCHA_CAPTCHA_CONFIG_H
#define XCAPTCHA_CAPTCHA_CONFIG_H
#include "config/detail/basic.h"
namespace captcha_config {

XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
class config : public XCAPTCHA_BASIC_CONFIG_TPL {

};

using config = config<>;

}

#endif //XCAPTCHA_CAPTCHA_CONFIG_H
