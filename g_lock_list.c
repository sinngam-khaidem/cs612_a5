#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include "lists.h"


typedef struct node {
    int key;
    struct node *next;
} node;

// pthread is the standard way to create threads and locks in C.
typedef struct {
    node *head;
    pthread_mutex_t lock;
} list;

static node *new_node(int key, node *next) {
    node *n = malloc(sizeof(node));
    n->key = key;
    n->next = next;
    return n;
}