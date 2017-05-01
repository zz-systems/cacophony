cmake_minimum_required(VERSION 3.3.2)
project(solowej-test)

include(../dependencies/gorynych/CMakeCommons)



set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fsanitize=undefined")
#set(CMAKE_BUILD_TYPE Debug)
set( CMAKE_VERBOSE_MAKEFILE on )
set(CMAKE_CXX_FLAGS "-std=c++14 -march=x86-64")


set(BRANCHES_TO_COMPILE
        sse2_branch
        sse3_branch
        ssse3_branch
        sse4_branch
        sse4fma_branch
        #avx1_branch
        avx2_branch
        )

set(BRANCH_MODULES
        "interpolation_test\;test_interpolation.cpp"
        "modules_test\;test_modules.cpp"
        "noise_test\;test_noise.cpp"
        "gather_test\;test_gather_randoms.cpp"
        "compiler_test\;test_compiler.cpp"
    )

prepare_branches()

list(APPEND SOURCE_FILES unit_test.cpp)

add_executable(solowej-test ${SOURCE_FILES})

enable_testing()
add_test(solowej-test solowej-test)

add_custom_target(test_solowej COMMAND ${CMAKE_CTEST_COMMAND}
        DEPENDS solowej-test)