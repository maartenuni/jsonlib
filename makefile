

STATIC_LIB=libjrep.a
CXXFLAGS:=$(CXXFLAGS) -std=c++0x -Wall -Wextra
CFLAGS:=$(CFLAGS) -Wall -Wextra

C_SRC=parse_utils.c lex.yy.c parser.tab.c
C_OBJ=parse_utils.o lex.yy.o parser.tab.o
CXX_SRC=jrep.cpp
CXX_OBJ=jrep.o
C_TEST_SRC=c_test.c
C_TEST_OBJ=c_test.o
CXX_TEST_SRC=cpp_test.cpp
CXX_TEST_OBJ=cpp_test.o

all:lib

lib: $(STATIC_LIB)

$(STATIC_LIB): $(C_OBJ) $(CXX_OBJ)
	$(AR) -cvq $@ $(C_OBJ) $(CXX_OBJ)

tests: cpp_test

c_test:

cpp_test:cpp_test.o $(STATIC_LIB)
	$(CXX) $(CXXFLAGS) -o $@ $< $(STATIC_LIB) 

cpp_test.o:

C_OBJ:$(C_SRC)
	$(CC) $(CCFLAGS) -c $@ $^

jrep.o: jrep.cpp jrep.h
	$(CXX) -c $(CXXFLAGS) jrep.cpp

lex.yy.c:lexer.l parser.tab.h
	flex lexer.l

parser.tab.h: parser.y
	bison -d parser.y
parser.tab.c:parser.y
	bison -d parser.y

clean:
	$(RM) $(C_OBJ)
	$(RM) jrep.o
	$(RM) lex.yy.h lex.yy.c
	$(RM) parser.tab.c parser.tab.h parser.output
	$(RM) $(CXX_TEST_OBJ)
	$(RM) $(C_TEST_OBJ)
	$(RM) $(STATIC_LIB)
	$(RM) cpp_test c_test
