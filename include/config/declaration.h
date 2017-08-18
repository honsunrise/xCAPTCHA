//
// Created by zhsyourai on 8/19/17.
//

#ifndef XCAPTCHA_DECLARATION_H
#define XCAPTCHA_DECLARATION_H
namespace captcha_config {

#define XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION                              \
    template<template<typename, typename, typename...> class ObjectType,   \
             template<typename, typename...> class ArrayType,              \
             class StringType, class BooleanType, class NumberIntegerType, \
             class NumberUnsignedType, class NumberFloatType,              \
             template<typename> class AllocatorType,                       \
             template<typename, typename = void> class JSONSerializer>

#define XCAPTCHA_BASIC_CONFIG_TPL                                          \
    basic_config<ObjectType, ArrayType, StringType, BooleanType,           \
    NumberIntegerType, NumberUnsignedType, NumberFloatType,                \
    AllocatorType, JSONSerializer>

}
#endif //XCAPTCHA_DECLARATION_H
