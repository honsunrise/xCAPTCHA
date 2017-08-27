set(SOURCE_FILES click_captcha.cpp click_captcha.h)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_library(click_captcha_plugin MODULE ${SOURCE_FILES})
SET_TARGET_PROPERTIES(click_captcha_plugin PROPERTIES PREFIX "")
if(Boost_FOUND)
    target_link_libraries(click_captcha_plugin xCAPTCHA dl ${Boost_LIBRARIES})
endif()