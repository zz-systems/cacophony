enable_testing()

file(GLOB ZACC_TEST_MAIN "${PROJECT_SOURCE_DIR}/*/zacc/*/test_main.cpp")
file(GLOB ZACC_TEST_ENTRYPOINT "${PROJECT_SOURCE_DIR}/*/zacc/*/test_entry_point.cpp")

file(GLOB tests "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

zacc_add_dispatched_tests(cacophony.tests
        TEST_MAIN ${ZACC_TEST_MAIN}
        TEST_ENTRYPOINT ${ZACC_TEST_ENTRYPOINT}
        BRANCHES "${branches}"
        INCLUDES ${CMAKE_SOURCE_DIR}/include
        SOURCES
            ${tests}
        )