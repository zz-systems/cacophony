cmake_minimum_required(VERSION 3.3.2)
project(solowej)

include(../dependencies/zacc/CMakeCommons)

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

set(BRANCH_MODULES "engine_dispatch\;platform/engine/engine.cpp")

set_openmp()
prepare_branches()


list(APPEND SOURCE_FILES ../src/libcacophony.cpp library/dllmain.cpp)

add_library(solowej SHARED ${SOURCE_FILES})
target_compile_definitions(solowej PUBLIC 
					-DFASTFLOAT 
					-DCOMPILE_FPU 
					-DCOMPILE_SSE2 
					-DCOMPILE_SSE3 
					-DCOMPILE_SSSE3 
					-DCOMPILE_SSE4 
					-DCOMPILE_SSE4FMA 
					-DCOMPILE_AVX1 
					-DCOMPILE_AVX2
					-DSOLOWEJ_EXPORTS
					)