cmake_minimum_required(VERSION 3.3.2)
project(solowej-examples)

include(../dependencies/gorynych/CMakeCommons)


set(CMAKE_CXX_FLAGS_DEBUG " -g -fsanitize=undefined -fsanitize=alignment")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
#set(CMAKE_BUILD_TYPE Debug)
set( CMAKE_VERBOSE_MAKEFILE on )
set(CMAKE_CXX_FLAGS "-std=c++14 -march=x86-64 -funroll-loops")

set(BRANCHES_TO_COMPILE
        fpu_branch
        sse2_branch
        sse3_branch
        ssse3_branch
        sse4_branch
        sse4fma_branch
        avx1_branch
        avx2_branch
        )

set(BRANCH_ADDITIONAL_DEFS -DFASTFLOAT)

#set(BRANCH_MODULES "objectdispatch\;../solowej/library/objectdispatch.cpp")
#prepare_branches()


list(APPEND SOURCE_FILES solowej-examples.cpp)


configure_file(config/granite.json ${CMAKE_CURRENT_BINARY_DIR}/config/granite.json)
configure_file(config/perlin_test.json ${CMAKE_CURRENT_BINARY_DIR}/config/perlin_test.json)
configure_file(config/planet.json ${CMAKE_CURRENT_BINARY_DIR}/config/planet.json)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/asyncplusplus ${CMAKE_BINARY_DIR}/async)

add_executable(solowej-examples ${SOURCE_FILES})
#disable OMP for now
#target_link_libraries(solowej-examples X11 jpeg png omp pthread)
target_link_libraries(solowej-examples X11 jpeg png pthread)
target_compile_definitions(solowej-examples PUBLIC -DCOMPILE_FPU -DCOMPILE_SSE2 -DCOMPILE_SSE3 -DCOMPILE_SSSE3 -DCOMPILE_SSE4 -DCOMPILE_SSE4FMA -DCOMPILE_AVX1 -DCOMPILE_AVX2)

target_link_libraries(solowej-examples Async++ solowej)