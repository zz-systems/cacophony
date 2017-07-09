cmake_minimum_required(VERSION 3.3.2)
project(cacophony)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies)

include(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/zacc/cmake/zacc.shared.cmake)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/zacc/include
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/json/src
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/CImg
)

# add app targets
add_subdirectory(src)

# add test targets
add_subdirectory(test)