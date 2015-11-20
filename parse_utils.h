
#ifndef PARSE_UTILS_H
#define PARSE_UTILS_H

typedef struct node {
    void* data;
    struct node* next;
}node;

node* list_create(void);
node* list_push_front(node* n, void* data);
node* list_push_back(node* n, void* data);
void list_destroy(node* front, void(*free_func)(void*));

#endif /*#define PARSE_UTILS_H*/
