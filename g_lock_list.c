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

int gl_find(void *lp, int k) {
    list *l = lp;
    pthread_mutex_lock(&l->lock);

    node *curr = l->head;
    while(curr->key < k)
        curr = curr->next;
    int found = (curr->key == k);

    pthread_mutex_unlock(&l->lock);
    return found;
}

int gl_insert(void *lp, int k) {
    list *l = lp;
    pthread_mutex_lock(&l->lock);

    node *pred = l->head;
    node *curr = pred->next;
    while (curr->key < k) {
        pred = curr;
        curr = curr->next;
    }
    int found = (curr->key == k);
    if (!found)
        pred->next = new_node(k, curr);

    pthread_mutex_unlock(&l->lock);
    return !found;
}

int gl_delete(void *lp, int k) {
    list *l = lp;
    pthread_mutex_lock(&l->lock);
    
    node *pred = l->head;
    node *curr = pred->next;
    while (curr->key < k) {
        pred = curr;
        curr = curr->next;
    }
    int found = (curr->key == k);
    if (found) {
        pred->next = curr->next;
        free(curr);
    }

    pthread_mutex_unlock(&l->lock);
    return found;
}