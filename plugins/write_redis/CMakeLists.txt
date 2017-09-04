set(SOURCE_FILES write_redis_plugin.cpp write_redis_plugin.h)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif ()

add_library(write_redis_plugin MODULE ${SOURCE_FILES})
SET_TARGET_PROPERTIES(write_redis_plugin PROPERTIES PREFIX "")
if (Boost_FOUND)
    target_link_libraries(write_redis_plugin xCAPTCHA dl redis3m ${Boost_LIBRARIES})
endif ()