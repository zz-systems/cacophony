cmake_minimum_required(VERSION 2.8.2)

project(fetch_json NONE)

include(ExternalProject)
ExternalProject_Add(asyncplusplus
        GIT_REPOSITORY    		https://github.com/nlohmann/json.git
        GIT_TAG           		master
        SOURCE_DIR        		"${CMAKE_CURRENT_BINARY_DIR}/json/src"
        BINARY_DIR        		"${CMAKE_CURRENT_BINARY_DIR}/json/build"

        CONFIGURE_COMMAND 		""
        BUILD_COMMAND     		""
        INSTALL_COMMAND   		""
        TEST_COMMAND      		""
        )