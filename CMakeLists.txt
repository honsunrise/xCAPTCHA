cmake_minimum_required(VERSION 3.8)
project(xCAPTCHA)

set(CMAKE_CXX_STANDARD 11)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(yaml-cpp REQUIRED)
include_directories(${yaml-cpp_INCLUDE_DIRS})


include_directories(include)
add_subdirectory(src)

set(SOURCE_FILES tools/main.cpp)
add_executable(captcha_view ${SOURCE_FILES})
target_link_libraries(captcha_view xCAPTCHA ${OpenCV_LIBS} ${yaml-cpp_LIBS})
