set(SOURCE_FILES background_plugin.cpp background_plugin.h)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif ()

add_library(background_plugin MODULE ${SOURCE_FILES})
SET_TARGET_PROPERTIES(background_plugin PROPERTIES PREFIX "")
if (Boost_FOUND)
    target_link_libraries(background_plugin xCAPTCHA dl ${Boost_LIBRARIES})
endif ()