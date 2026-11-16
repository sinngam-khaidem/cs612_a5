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

int hoh_find(void *lp, int k) {
    list *l = lp;

    pthread_mutex_lock(&l->head->lock);
    node *pred = l->head;
    pthread_mutex_lock(&pred->next->lock);

    node *curr = pred->next;
    while (curr->key < k) {
        pthread_mutex_unlock(&pred->lock);
        pred = curr;
        pthread_mutex_lock(&curr->next->lock);
        curr = pred->next;
    }
    int found = curr->key == k;

    pthread_mutex_unlock(&pred->lock);
    pthread_mutex_unlock(&curr->lock);

    return found;
}

int hoh_insert(void *lp, int k) {
    list *l = lp;
    pthread_mutex_lock(&l->head->lock);
    node *pred = l->head;
    pthread_mutex_lock(&pred->next->lock); 
    node *curr = pred->next;

    while (curr->key < k) {
        pthread_mutex_unlock(&pred->lock);
        pred = curr;
        pthread_mutex_lock(&curr->next->lock);
        curr = pred->next;
    }

    int found = curr->key == k;
    if (!found)
        pred->next = new_node(k, curr);

    pthread_mutex_unlock(&pred->lock);
    pthread_mutex_unlock(&curr->lock);
    return !found;
}

int hoh_delete(void *lp, int k) {
    list *l = lp;
    pthread_mutex_lock(&l->head->lock);
    node *pred = l->head;
    pthread_mutex_lock(&pred->next->lock);
    node *curr = pred->next;

    while (curr->key < k) {
        pthread_mutex_unlock(&pred->lock);
        pred = curr;
        pthread_mutex_lock(&curr->next->lock);
        curr = curr->next;
    }
    int found = curr->key == k;
    
    if (found) {
        pred->next = curr->next;
        pthread_mutex_unlock(&curr->lock);
        free_node(curr);
        pthread_mutex_unlock(&pred->lock);
    } else {
        pthread_mutex_unlock(&pred->lock);
        pthread_mutex_unlock(&curr->lock);
    }
    return found;
}
