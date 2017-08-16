set(SOURCE_FILES distortion_plugin.cpp distortion_plugin.h)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_library(distortion_plugin SHARED ${SOURCE_FILES})
SET_TARGET_PROPERTIES(distortion_plugin PROPERTIES PREFIX "")
if(Boost_FOUND)
    target_link_libraries(distortion_plugin xCAPTCHA yaml-cpp dl ${Boost_LIBRARIES})
endif()