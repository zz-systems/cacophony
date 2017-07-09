cmake_minimum_required(VERSION 3.3.2)
project(cacophony.dependencies NONE)

add_subdirectory(zacc)
add_subdirectory(json)

include_directories(cimg)
include_directories(json/src)

if(gtest_SOURCE_DIR)
    message(STATUS "gtest variables already defined. Skipping. ")
    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
else()
    add_subdirectory(googletest)
endif()