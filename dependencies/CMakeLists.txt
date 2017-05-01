cmake_minimum_required(VERSION 3.3.2)
project(cacophony.dependencies NONE)

add_subdirectory(zacc)
#add_subdirectory(googletest)
add_subdirectory(json)

include_directories(cimg)
include_directories(json/src)