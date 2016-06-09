cmake_minimum_required(VERSION 3.3.2)
project(solowej)

include(../gorynych/CMakeCommons)


set(CMAKE_CXX_FLAGS_DEBUG " -g -fsanitize=undefined -fsanitize=alignment")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)
set( CMAKE_VERBOSE_MAKEFILE on )
set(CMAKE_CXX_FLAGS "-std=c++14 -fPIC -march=x86-64")

set(BRANCHES_TO_COMPILE
        fpu_branch
        sse2_branch
        sse3_branch
        ssse3_branch
        sse4_branch
        sse4fma_branch
        avx1_branch
        avx2_branch)

set(BRANCH_ADDITIONAL_DEFS -DFASTFLOAT)

set(BRANCH_MODULES "solowej_objectdispatch\;library/objectdispatch.cpp")
prepare_branches()


list(APPEND SOURCE_FILES library/solowejlib.cpp library/dllmain.cpp)

#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../asyncplusplus ${CMAKE_BINARY_DIR}/solowej/async)

add_library(solowej SHARED ${SOURCE_FILES})
target_compile_definitions(solowej PUBLIC -DFASTFLOAT -DCOMPILE_FPU -DCOMPILE_SSE2 -DCOMPILE_SSE3 -DCOMPILE_SSSE3 -DCOMPILE_SSE4 -DCOMPILE_SSE4FMA -DCOMPILE_AVX1 -DCOMPILE_AVX2)

target_link_libraries(solowej Async++)