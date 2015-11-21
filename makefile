

PROG=test
CXXFLAGS:=$(CXXFLAGS) -std=c++0x -Wall -Wextra
CFLAGS:=$(CFLAGS) -Wall -Wextra

C_SRC=parse_utils.c lex.yy.c parser.tab.c
C_OBJ=parse_utils.o lex.yy.o parser.tab.o

all:$(PROG)

$(PROG): jrep.o $(C_OBJ)
	$(CC) $(CCFLAGS) -o $@ $^ -lstdc++

C_OBJ:$(C_SRC)
	$(CC) $(CCFLAGS) -c $@ $<

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
