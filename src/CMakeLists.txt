file(GLOB_RECURSE SOURCE_FILES plugins/*)
set(SOURCE_FILES ${SOURCE_FILES} ../include/processor_plugin_interface.h captcha_plugin_stub.h captcha_plugin_stub.cpp
        ../include/captcha_context.h ../include/captcha_config.h  ../include/captcha.h captcha_context.cpp captcha.cpp)

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost 1.58.0 COMPONENTS iostreams date_time filesystem system program_options thread)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_library(xCAPTCHA ${SOURCE_FILES})

if(Boost_FOUND)
    target_link_libraries(xCAPTCHA yaml-cpp dl ${Boost_LIBRARIES})
endif()