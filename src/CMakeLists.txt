file(GLOB_RECURSE SOURCE_FILES plugins/*)
set(SOURCE_FILES ${SOURCE_FILES} ../include/processor_plugin_interface.h
        ../include/captcha_context.h ../include/captcha_config.h  ../include/captcha.h captcha_context.cpp captcha.cpp)
add_library(xCAPTCHA ${SOURCE_FILES})
target_link_libraries(xCAPTCHA yaml-cpp)