cmake_minimum_required(VERSION 3.8)
project(xCAPTCHA)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)
add_subdirectory(src)

set(SOURCE_FILES tools/main.cpp)
add_executable(captcha_view ${SOURCE_FILES})