
%{
#include <stdio.h>
#include "jrep.h"
#include "parse_utils.h"

void yyerror(const char*);
%}


%union {
    j_val*              val;
    j_object*           obj;
    node*               pairs;
    key_value_pair*     pair;
    j_array*            arr;
    node*               elems;
    j_string*           str;
    j_bool*             boolean;
    j_number*           num;
    j_null*             null;
    double              flt;
}

%type <val>     value
%type <obj>     object
%type <pairs>   members
%type <pair>    pair
%type <elems>   elements
%type <str>     string
%type <arr>     array
%type <num>     number

%token <null>       TOK_NULL
%token <boolean>    TOK_TRUE TOK_FALSE
%token <str>        TOK_STRLIT
%token <flt>        TOK_NUM

%%

start:
    | object            {
                            fprintf(stdout, "%s\n", j_val_representation((j_val*)$1) );
                            j_val_destroy((j_val*)$1);
                        }
    ;

object: '{'   '}'       {
                            $$ = j_object_create();
                            if (!$$)
                                yyerror("out of mem");
                        }
    |   '{' members '}' {
                            $$ = j_object_create();
                            if (!$$)
                                yyerror("out of mem");
                            node* head = $2;
                            node* n = head; 
                            do {
                                key_value_pair* p = n->data;
                                j_string* s = p->string;
                                j_val* val = p->value;
                                j_object_add_value($$, j_string_get_value(s), val);
                                n = n->next;
                            } while (n);
                            list_destroy(head, (list_data_free_func) j_val_destroy);
                        }
    ;

members: pair           {
                            $$ = list_push_front(NULL, $1);
                        }
    | members ',' pair  {
                            $$ = list_push_front($1, $3);
                        }
    ;

pair: TOK_STRLIT ':' value  {
                                key_value_pair* p = malloc(sizeof(key_value_pair));
                                if (p) {
                                    p->string = $1;
                                    p->value = $3;
                                }
                                $$ = p;
                            }
    ;

array : '['   ']'           {$$ = j_array_create();}
    |   '[' elements ']'    {
                                $$ = j_array_create();
                                node* head = $2;
                                head = list_reverse(head);
                                node* n = head;
                                do {
                                    j_val* v = n->data;
                                    j_array_add_value($$, v);
                                    n = n->next;
                                } while(n);
                                list_destroy(head, (list_data_free_func) j_val_destroy);
                            }
    ;

elements: value             {
                                $$ = list_push_front(NULL, $1);
                            }
    | elements ',' value    {
                                if ($1)
                                    $$ = list_push_front($1, $3);
                            }
    ;

value: string       { $$ = (j_val*) $1;}
    | number        { $$ = (j_val*) $1;}
    | object        { $$ = (j_val*) $1;}
    | array         { $$ = (j_val*) $1;}
    | TOK_TRUE      { $$ = (j_val*) j_bool_create_bool(1); }
    | TOK_FALSE     { $$ = (j_val*) j_bool_create_bool(0); }
    | TOK_NULL      { $$ = (j_val*) j_null_create();}

string : TOK_STRLIT {
                        $$ = $1;
                    }

number : TOK_NUM    {
                        double d = $1;
                        $$ = j_number_create(d);
                    }

%%

int main() {
    yyparse();
    return 0;
}

void yyerror(const char* error) {
    fprintf(stderr, "Unable to parse input: %s\n", error);
}
