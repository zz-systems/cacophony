set(CMAKE_POSITION_INDEPENDENT_CODE ON)

zacc_add_dispatched_library(cacophony
        BRANCHES "${branches}"
        INCLUDES
            ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/dependencies/zacc/include
        ENTRYPOINT
            ${CMAKE_SOURCE_DIR}/src/cacophony_branch_entrypoint.cpp
        SOURCES
            ${CMAKE_SOURCE_DIR}/src/libcacophony.cpp
            ${CMAKE_SOURCE_DIR}/dependencies/zacc/src/dllmain.cpp
        )

if(CLANG_CL)
    target_compile_options(cacophony PUBLIC -Xclang -fno-delayed-template-parsing)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # requires XQuartz
    include_directories(AFTER "/usr/X11/include")
    link_directories("/usr/X11/lib" "/usr/X11R6/lib")
endif()

add_executable(cacophony.examples cacophony_examples.cpp)
add_executable(cacophony.examples.headless cacophony_examples.cpp)
target_compile_definitions(cacophony.examples.headless PUBLIC "CACOPHONY_EXAMPLES_HEADLESS=1")

if(WIN32)
    target_link_libraries(cacophony.examples cacophony)
    target_link_libraries(cacophony.examples.headless cacophony)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # requires XQuartz
    target_link_libraries(cacophony.examples X11 cacophony)
    target_link_libraries(cacophony.examples.headless cacophony)
else()
    target_link_libraries(cacophony.examples X11 jpeg png cacophony)
    target_link_libraries(cacophony.examples.headless cacophony)
endif()


add_custom_command(TARGET "cacophony.examples" POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/examples
        $<TARGET_FILE_DIR:cacophony.examples>/config/
        )

add_custom_command(TARGET "cacophony.examples.headless" POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/examples
        $<TARGET_FILE_DIR:cacophony.examples.headless>/config/
        )

#configure_file(../examples/granite.json $<TARGET_FILE_DIR:cacophony.examples>/config/granite.json)
#configure_file(../examples/planet.json ${CMAKE_CURRENT_BINARY_DIR}/config/planet.json)