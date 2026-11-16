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

// Helpers
static node *new_node(int key, node *next) {
    node *n = malloc(sizeof(node));
    n->key = key;
    n->next = next;
    return n;
}

// Find where key k belongs.
static void locate(list *l, int k, node **pred, node **curr) {
    *pred = l->head;
    *curr = l->head->next;
    while ((*curr)->key < k) {
        *pred = *curr;
        *curr = (*curr)->next;
    }
}

// Create a new empty list with sentinel nodes.
void *gl_new(void) {
    list *l = malloc(sizeof(list));
    l->head = new_node(INT_MIN, new_node(INT_MAX, NULL));

    pthread_mutex_init(&l->lock, NULL);
    return l;
}

// Destroy a list and free all memory.
void gl_free(void *lp) {
    list *l = lp;
    node *next;
    
    for (node *n = l->head; n; n = next) {
        next = n->next;
        free(n);
    }
    pthread_mutex_destroy(&l->lock);
    free(l);
}

