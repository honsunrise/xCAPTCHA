if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

set(SOURCE_FILES main.cpp)
add_executable(captcha_view ${SOURCE_FILES})
target_link_libraries(captcha_view xCAPTCHA ${OpenCV_LIBS} ${yaml-cpp_LIBS})

if(Boost_FOUND)
    target_link_libraries(captcha_view ${Boost_LIBRARIES})
endif()