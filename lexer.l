
%option noyywrap

D                       [0-9]
E                       [Ee][+-]?{D}+
L                       [a-zA-Z_]
H                       [a-fA-F0-9]
NO_STR_CHAR             [\\/"\b\f\n\r\t]

%{
#include <stdio.h>
#include <string.h>
#include "parse_utils.h"
#define  INSIDE_LEXER
#include "jrep.h"
#undef   INSIDE_LEXER
#include "parser.tab.h"
%}

%%

"true"                          {return TOK_TRUE;}
"false"                         {return TOK_FALSE;}
"null"                          {return TOK_NULL;}

([-])?{D}+([.]{D}+)?{E}?        {
                                    yylval.flt = atof(yytext);
                                    return  TOK_NUM;
                                }
\"(\\.|[^\\"\b\f\n\r\t])*\"     {
                                    char* temp;
                                    yylval.str = j_string_create();
                                    /*remove quotes from match*/
                                    temp = strndup(&yytext[1], yyleng-2);
                                    j_string_assign(yylval.str, temp);
                                    free(temp);
                                    return TOK_STRLIT;
                                }

"{" |
"}" |
":" |
"[" |
"]" |
"," |
"-"                             {return yytext[0];}

[[:space:]]                     {}

%%

void lexer_scan_string(const char* string)
{
    YY_BUFFER_STATE state = yy_scan_string(string);
    //free(state);
}

void lexer_scan_bytes(const char* bytes, int length)
{
    YY_BUFFER_STATE state = yy_scan_bytes(bytes, length);
    //free(state);
}

void lexer_scan_buffer(char* bytes, int length)
{
    YY_BUFFER_STATE state = yy_scan_buffer(bytes, length);
    //free(state);
}
