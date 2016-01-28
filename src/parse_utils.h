
#ifndef PARSE_UTILS_H
#define PARSE_UTILS_H

#include "jrep.h"

typedef struct node {
    void* data;
    struct node* next;
}node;

typedef void(*list_data_free_func)(void*) ;

node* list_create(void);
node* list_push_front(node* n, void* data);
node* list_push_back(node* n, void* data);
node* list_reverse(node* n);
void list_destroy(node* front, void(*free_func)(void*));


typedef struct key_value_pair {
    j_string*   string;
    j_val*      value;
}key_value_pair;



#endif /*#define PARSE_UTILS_H*/
