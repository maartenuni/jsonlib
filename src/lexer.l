
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

YY_BUFFER_STATE our_state;

void lexer_scan_string(const char* string)
{
    our_state = yy_scan_string(string);
}

void lexer_scan_bytes(const char* bytes, int length)
{
    our_state = yy_scan_bytes(bytes, length);
}

void lexer_scan_buffer(char* bytes, int length)
{
    our_state = yy_scan_buffer(bytes, length);
}

void lexer_destroy_buffer()
{
    yy_delete_buffer(our_state);
    our_state = NULL;
}
