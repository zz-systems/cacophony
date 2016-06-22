cmake_minimum_required(VERSION 3.3.2)
project(solowej-examples)

include(../dependencies/gorynych/CMakeCommons)

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

list(APPEND SOURCE_FILES solowej-examples.cpp)


configure_file(config/granite.json ${CMAKE_CURRENT_BINARY_DIR}/config/granite.json)
configure_file(config/perlin_test.json ${CMAKE_CURRENT_BINARY_DIR}/config/perlin_test.json)
configure_file(config/planet.json ${CMAKE_CURRENT_BINARY_DIR}/config/planet.json)

set_openmp()

add_executable(solowej-examples ${SOURCE_FILES})

target_link_libraries(solowej-examples X11 jpeg png solowej)
target_compile_definitions(solowej-examples PUBLIC -DCOMPILE_FPU -DCOMPILE_SSE2 -DCOMPILE_SSE3 -DCOMPILE_SSSE3 -DCOMPILE_SSE4 -DCOMPILE_SSE4FMA -DCOMPILE_AVX1 -DCOMPILE_AVX2)
