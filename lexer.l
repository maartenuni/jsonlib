
D                       [0-9]
E                       [Ee][+-]?{D}+
L                       [a-zA-Z_]
H                       [a-fA-F0-9]
NO_STR_CHAR             [\\/"\b\f\n\r\t]

%{
#include <stdio.h>
#include "y.tab.h"
%}

%%

"true"                          {return TOK_TRUE;}
"false"                         {return TOK_FALSE;}
"null"                          {return TOK_NULL;}

{D}+([.]{D}+)?{E}?              {return TOK_NUM;}
\"(\\.|[^\\/"\b\f\n\r\t])*\"    {return TOK_STRLIT;}

"{" |
"}" |
":" |
"[" |
"]" |
"," |
"-"                             {return yytext[0];}

[[:space:]]                     {}

%%
