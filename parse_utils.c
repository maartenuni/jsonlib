
#include <stdlib.h>
#include "parse_utils.h"

node* list_create(void) {
    node* n = malloc(sizeof(node));
    if(n)
        n->next = NULL;
    return n;
}

node* list_push_front(node* list, void* data)
{
    node* n = list_create();
    if (n) {
        n->next = list;
        n->data = data;
    }
    return n;
}

node* list_push_back(node* list, void* data)
{
    node* n = list_create();
    if (n)
        n->data = data;
    if (list)
        list->next = n;
    return n;
}

node* list_reverse(node* root)
{
    node* new_root = NULL;
    while (root) {
        node* next = root->next;
        root->next = new_root;
        new_root = root;
        root = next;
    }
    return new_root;
}

void list_destroy(node* n, void (*free_func)(void* data)) {
    while(n) {
        node* temp = n;
        free_func(temp->data);
        n = temp->next;
        free(temp);
    }
}


#ifdef TEST_LIST

#include <stdio.h>

int main() {

    node* n = NULL;
    node* next = NULL;
    int i;
    
    for (i = 0; i < 10; i++) {
        int* val = malloc(sizeof(int));
        *val = i;
        next = list_push_back(next, val);
        if (i == 0)
            // store head
            n = next;
    }
    for (i = 0; i < 10; i++) {
        int* val = malloc(sizeof(int));
        *val = i;
        n = list_push_front(n, val);
    }

    next = n;
    do {
        fprintf(stdout,"%d\n", * (int*) next->data); 
        next = next->next;
    }while(next);

    list_destroy(n, free);
    return 0;
}

#endif
