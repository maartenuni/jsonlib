

find_package(BISON REQUIRED)
find_package(FLEX  REQUIRED)

BISON_TARGET(MyParser parser.y "${PROJECT_BINARY_DIR}/src/parser.tab.c")
FLEX_TARGET(MyScanner lexer.l "${PROJECT_BINARY_DIR}/src/lex.yy.c")

ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)


add_library(jrep SHARED
            jrep.cpp
            jrep.h
            lex.yy.c
            parser.tab.c
            parse_utils.c
            parse_utils.h)


add_library(jrep-static STATIC
            jrep.cpp
            jrep.h
            lex.yy.c
            parser.tab.c
            parse_utils.c
            parse_utils.h)

