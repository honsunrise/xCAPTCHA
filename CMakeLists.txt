cmake_minimum_required(VERSION 3.0)
project(xCAPTCHA)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(yaml-cpp REQUIRED)
include_directories(${yaml-cpp_INCLUDE_DIRS})

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.58.0 COMPONENTS iostreams date_time filesystem system program_options thread)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif ()

include_directories(include)
include_directories(src)

file(GLOB_RECURSE sources "src/[a-zA-Z_]*.cpp")
file(GLOB_RECURSE public_headers "include/[a-zA-Z_]*.h")
file(GLOB_RECURSE private_headers "src/[a-zA-Z_]*.h")

set(SOURCE_FILES ${sources} ${public_headers} ${private_headers})

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
        add_subdirectory(${curdir}/${subdir})
    endforeach ()
endfunction(add_plugins_dir)

add_plugins_dir(plugins)

function(add_apps_dir curdir)
    file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${curdir} ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/*)
    set(dirlist "")
    foreach (child ${children})
        if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/${child})
            list(APPEND dirlist ${child})
        endif ()
    endforeach ()
    foreach (subdir ${dirlist})
        add_subdirectory(${curdir}/${subdir})
    endforeach ()
endfunction(add_apps_dir)

add_apps_dir(apps)