
cmake_minimum_required(VERSION 2.6)
project(jrep C CXX) 

set (JREP_VERSION_MAJOR 0)
set (JREP_VERSION_MINOR 1)
set (JREP_VERSION_MICRO 1)
set (JREP_VERSION_STRING
     "\"jrep-${JREP_VERSION_MAJOR}.${JREP_VERSION_MINOR}.${JREP_VERSION_MICRO}\""
    )


option(BUILD_TESTS "Build test examples" ON)

configure_file(
    ${PROJECT_SOURCE_DIR}/jrep-config.h.in
    ${PROJECT_BINARY_DIR}/jrep-config.h
    )

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src")

add_subdirectory(src)
if (BUILD_TESTS)
    add_subdirectory(tests)
endif(BUILD_TESTS)

