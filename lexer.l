
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
#include "jrep.h"
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
\"(\\.|[^\\/"\b\f\n\r\t])*\"    {
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
