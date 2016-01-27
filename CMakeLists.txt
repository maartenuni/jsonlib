
cmake_minimum_required(VERSION 2.6)
project(jrep C CXX) 

set (JREP_VERSION_MAJOR 0)
set (JREP_VERSION_MINOR 1)
set (JREP_VERSION_MICRO 1)
set (JREP_VERSION_STRING
     "\"jrep-${JREP_VERSION_MAJOR}.${JREP_VERSION_MINOR}.${JREP_VERSION_MICRO}\""
    )

find_package(BISON REQUIRED)
find_package(FLEX  REQUIRED)


configure_file(
    ${PROJECT_SOURCE_DIR}/jrep-config.h.in
    ${PROJECT_BINARY_DIR}/jrep-config.h
    )

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")

BISON_TARGET(MyParser parser.y "${PROJECT_BINARY_DIR}/parser.tab.c")
FLEX_TARGET(MyScanner lexer.l "${PROJECT_BINARY_DIR}/lex.yy.c")
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)


add_library(jrep SHARED
            jrep.cpp
            lex.yy.c
            parser.tab.c
            parse_utils.c)


add_library(jrep-static STATIC
            jrep.cpp
            lex.yy.c
            parser.tab.c
            parse_utils.c)

add_executable(cppsmalltest smalltest.cpp)
add_executable(csmalltest smalltest.c)
add_executable(cpptest cpp_test.cpp)

target_link_libraries(cppsmalltest jrep)
target_link_libraries(csmalltest jrep)
target_link_libraries(cpptest jrep)
