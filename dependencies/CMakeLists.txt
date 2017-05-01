macro(fetch_dependency dependency)
    configure_file(fetch_${dependency}.cmake ${CMAKE_CURRENT_BINARY_DIR}/${dependency}/download/CMakeLists.txt)

    execute_process(COMMAND ${CMAKE_COMMAND} .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${dependency}/download )
    if(result)
        message(FATAL_ERROR "CMake step for ${dependency} failed: ${result}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${dependency}/download )
    if(result)
        message(FATAL_ERROR "Build step for ${dependency} failed: ${result}")
    endif()

    # Add dependency directly to our build.
    add_subdirectory(
            ${CMAKE_CURRENT_BINARY_DIR}/${dependency}/src
            ${CMAKE_CURRENT_BINARY_DIR}/${dependency}/build
    )
endmacro()

# configure googletest =================================================================================================

fetch_dependency(googletest)

if(MSVC)
	target_compile_definitions(gtest PUBLIC -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
	target_compile_definitions(gtest_main PUBLIC -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
endif()
# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif()

# configure zacc =======================================================================================================

fetch_dependency(zacc)

# configure cimg =======================================================================================================

fetch_dependency(cimg)

# configure json =======================================================================================================

fetch_dependency(json)