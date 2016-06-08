cmake_minimum_required(VERSION 3.5)
project(solowej)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -mtune=generic")

add_subdirectory(solowej/solowej)
add_subdirectory(solowej/solowej-examples)
add_subdirectory(solowej/solowej-test)

add_subdirectory(solowej/gorynych/gorynych-test)