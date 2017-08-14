set(SOURCE_FILES ../include/processor_plugin_interface.h captcha_plugin_stub.h captcha_plugin_stub.cpp
        ../include/captcha_context.h ../include/config/config.h ../include/config/iterator.h
        ../include/captcha.h captcha_context.cpp captcha.cpp)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif ()

add_library(xCAPTCHA SHARED ${SOURCE_FILES})

if (Boost_FOUND)
    target_link_libraries(xCAPTCHA yaml-cpp dl ${Boost_LIBRARIES})
endif ()

function(add_plugins_dir curdir)
    file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${curdir} ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/*)
    set(dirlist "")
    foreach (child ${children})
        if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/${child})
            list(APPEND dirlist ${child})
        endif ()
    endforeach ()
    foreach (subdir ${dirlist})
        add_subdirectory(plugins/${subdir})
    endforeach ()
endfunction(add_plugins_dir)

add_plugins_dir(plugins)


