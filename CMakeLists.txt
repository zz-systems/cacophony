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

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(STATUS "ADD libdl, GNU")
    link_libraries(zacc.system.loader -Wl,--no-as-needed -ldl)
elseif ((CMAKE_CXX_COMPILER_ID MATCHES "Clang") AND (NOT APPLE) AND (NOT MSVC))
    message(STATUS "ADD libdl, Clang")
    link_libraries(zacc.system.loader -Wl,--no-as-needed -ldl)
else()
    message(STATUS "SKIP libdl")
endif()

enable_testing()

# add app targets
add_subdirectory(src)

# add test targets
add_subdirectory(test)