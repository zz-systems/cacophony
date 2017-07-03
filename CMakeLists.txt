cmake_minimum_required(VERSION 3.3.2)
project(cacophony)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies)

include(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/zacc/cmake/shared.cmake)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/zacc/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/json/src
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/CImg
)

foreach(branch ${branches})

    add_library("cacophony.${branch}" OBJECT src/cacophony_branch_entrypoint.cpp)
    add_dependencies("cacophony.${branch}" "zacc.generate.${branch}.types" "zacc.generate.${branch}.tests" )

    foreach(flag ${branch_flags_${branch}})
        if(CLANG_CL)
            set(flag "-Xclang ${flag}")
        endif()

        target_compile_options("cacophony.${branch}" PUBLIC ${branch_flags_${branch}})
    endforeach()

    target_include_directories(cacophony.${branch} PUBLIC "${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/dependencies/zacc/include")

    foreach(def ${branch_defs_${branch}})
        target_compile_definitions(cacophony.${branch}  PUBLIC "${def}")
    endforeach()

    list(APPEND sources $<TARGET_OBJECTS:cacophony.${branch}>)
endforeach()

add_library(cacophony SHARED ${sources} src/libcacophony.cpp dependencies/zacc/src/dllmain.cpp)
target_link_libraries(cacophony zacc.system)

target_compile_definitions(cacophony PUBLIC ZACC_FAST_FLOAT=false)
foreach(def ${generic_build_defs})
    target_compile_definitions(cacophony  PUBLIC "${def}")
endforeach()

#include(examples/CMakeLists.txt)
#include(test/CMakeLists.txt)

configure_file(examples/granite.json ${CMAKE_CURRENT_BINARY_DIR}/config/granite.json)
configure_file(examples/planet.json ${CMAKE_CURRENT_BINARY_DIR}/config/planet.json)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # requires XQuartz
    include_directories(AFTER "/usr/X11/include")
    link_directories("/usr/X11/lib" "/usr/X11R6/lib")
endif()

add_executable(cacophony.examples src/cacophony_examples.cpp)

if(WIN32)
    target_link_libraries(cacophony.examples cacophony)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # requires XQuartz
    target_link_libraries(cacophony.examples X11 cacophony)
else()
    target_link_libraries(cacophony.examples X11 jpeg png cacophony)
endif()