set(SOURCE_FILES noise_plugin.cpp noise_plugin.h)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_library(noise_plugin MODULE ${SOURCE_FILES})
SET_TARGET_PROPERTIES(noise_plugin PROPERTIES PREFIX "")
if(Boost_FOUND)
    target_link_libraries(noise_plugin xCAPTCHA yaml-cpp dl ${Boost_LIBRARIES})
endif()