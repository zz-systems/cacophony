cmake_minimum_required(VERSION 3.3.2)
project(solowej)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -mtune=generic")

add_subdirectory(solowej)
add_subdirectory(solowej-examples)
add_subdirectory(solowej-test)

add_subdirectory(dependencies/gorynych)
#add_subdirectory(dependencies/gorynych/gorynych-test)
