set(SOURCE_FILES filling_plugin.cpp filling_plugin.h)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_library(filling_plugin MODULE ${SOURCE_FILES})
SET_TARGET_PROPERTIES(filling_plugin PROPERTIES PREFIX "")
if(Boost_FOUND)
    target_link_libraries(filling_plugin xCAPTCHA yaml-cpp dl ${Boost_LIBRARIES})
endif()