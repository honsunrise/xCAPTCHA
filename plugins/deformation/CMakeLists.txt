set(SOURCE_FILES deformation_plugin.cpp deformation_plugin.h)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif ()

add_library(deformation_plugin MODULE ${SOURCE_FILES})
SET_TARGET_PROPERTIES(deformation_plugin PROPERTIES PREFIX "")
if (Boost_FOUND)
    target_link_libraries(deformation_plugin xCAPTCHA dl ${Boost_LIBRARIES})
endif ()