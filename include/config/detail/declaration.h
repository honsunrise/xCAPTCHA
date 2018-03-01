//
// Created by zhsyourai on 8/19/17.
//

#ifndef XCAPTCHA_DECLARATION_H
#define XCAPTCHA_DECLARATION_H
namespace captcha_config {

    template<typename = void, typename = void>
    struct adl_serializer;

    template<template<typename U, typename V, typename... Args> class ObjectType =
    std::map,
            template<typename U, typename... Args> class ArrayType = std::vector,
            class StringType = std::string, class BooleanType = bool,
            class NumberIntegerType = std::int64_t,
            class NumberUnsignedType = std::uint64_t,
            class NumberFloatType = double,
            template<typename U> class AllocatorType = std::allocator,
            template<typename T, typename SFINAE = void> class Serializer =
            adl_serializer>
    class basic_config;

    template<template<typename U, typename V, typename... Args> class ObjectType =
    std::map,
            template<typename U, typename... Args> class ArrayType = std::vector,
            class StringType = std::string, class BooleanType = bool,
            class NumberIntegerType = std::int64_t,
            class NumberUnsignedType = std::uint64_t,
            class NumberFloatType = double,
            template<typename U> class AllocatorType = std::allocator,
            template<typename T, typename SFINAE = void> class Serializer =
            adl_serializer>
    class basic_config_define;

#define XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION                              \
    template<template<typename, typename, typename...> class ObjectType,   \
             template<typename, typename...> class ArrayType,              \
             class StringType, class BooleanType, class NumberIntegerType, \
             class NumberUnsignedType, class NumberFloatType,              \
             template<typename> class AllocatorType,                       \
             template<typename, typename = void> class Serializer>

#define XCAPTCHA_BASIC_CONFIG_TPL                                          \
    basic_config<ObjectType, ArrayType, StringType, BooleanType,           \
    NumberIntegerType, NumberUnsignedType, NumberFloatType,                \
    AllocatorType, Serializer>

#define XCAPTCHA_BASIC_CONFIG_DEFINE_TPL                                   \
    basic_config_define<ObjectType, ArrayType, StringType, BooleanType,    \
    NumberIntegerType, NumberUnsignedType, NumberFloatType,                \
    AllocatorType, Serializer>

}
#endif //XCAPTCHA_DECLARATION_H
