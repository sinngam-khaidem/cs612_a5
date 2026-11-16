#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include "lists.h"

typedef struct node {
    int key;
    struct node *next;
    pthread_mutex_t lock;
} node;

typedef struct {
    node *head;
} list;

static node *new_node(int key, node *next) {
    node *n = malloc(sizeof(node));
    n->key = key;
    n->next = next;

    pthread_mutex_init(&n->lock, NULL);
    return n;
}

static void free_node(node *n) {
    pthread_mutex_destroy(&n->lock);
    free(n);
}

void *hoh_new(void) {
    list *l = malloc(sizeof(list));
    l->head = new_node(INT_MIN, new_node(INT_MAX, NULL));
    return l;
}

void hoh_free(void *lp) {
    list *l = lp;
    for (node *n = l->head, *next; n; n = next) {
        next = n->next;
        free_node(n);
    }
    free(l);
}