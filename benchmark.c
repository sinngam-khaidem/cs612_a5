#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "lists.h"

#define KEYS 1024   // keys are random numbers from 0 to 1023
#define MAX_THREADS 64 

typedef struct {
    const char *name;
    void *(*new)(void);
    void (*free)(void *);
    int (*find)(void *, int);
    int (*insert)(void *, int);
    int (*delete)(void *, int);
} list_ops;

static const list_ops LISTS[] = {
    {"global", gl_new, gl_free, gl_find, gl_insert, gl_delete},
    {"hoh", hoh_new, hoh_free, hoh_find, hoh_insert, hoh_delete},
};

static const int THREADS[] = {1, 2, 4, 8, 16, 32, 64};

// A worker thread runs random operations on a list until stop is set.
typedef struct {
    const list_ops *ops; // which list implementation to use
    void *list; // the list to operate on
    unsigned seed; // the thread's random seed
    long count; // number of operations performed, filled in at the end
} worker;

// The main thread uses stop flag to tell every worter to finish.
static atomic_bool stop;

static double now(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

// Each worker thread runs random operations on the list until stop is set.
// 80% find, 10% insert, 10% delete on random keys until stop is set.
static void *run(void *arg) {
    worker *w = arg;
    long n = 0;
    
    while (!atomic_load(&stop)) {
        int r = rand_r(&w->seed);
        int k = r % KEYS, op = (r / KEYS) % 10;
        if (op < 8)
            w->ops->find(w->list, k);
        else if (op == 8)
            w->ops->insert(w->list, k);
        else
            w->ops->delete(w->list, k);
        n++;
    }
    w->count = n;
    return NULL;
}

// Runs nthreads on a half-full list for 1 s; returns ops/sec.
static double trial(const list_ops *ops, int nthreads) {
    void *l = ops->new();
    for (int k = 0; k < KEYS; k += 2)
        ops->insert(l, k);

    pthread_t tids[MAX_THREADS];
    worker workers[MAX_THREADS];
    atomic_store(&stop, 0);

    double t0 = now();
    for (int i = 0; i < nthreads; i++) {
        workers[i] = (worker){ops, l, i + 1, 0};
        pthread_create(&tids[i], NULL, run, &workers[i]);
    }
    sleep(1);
    atomic_store(&stop, 1);

    long total = 0;
    for (int i = 0; i < nthreads; i++) {
        pthread_join(tids[i], NULL);
        total += workers[i].count;
    }
    double t1 = now();

    ops->free(l);
    return total / (t1 - t0);
}

int main(void) {
    int nlists = sizeof LISTS / sizeof LISTS[0];
    int nthreads = sizeof THREADS / sizeof THREADS[0];

    FILE *out = fopen("results.csv", "w");
    fprintf(out, "threads,list,ops_per_sec\n");
    for (int t = 0; t < nthreads; t++) {
        for (int i = 0; i < nlists; i++) {
            double ops_per_sec = trial(&LISTS[i], THREADS[t]);
            printf("%2d threads  %-6s  %12.0f ops/sec\n", THREADS[t], LISTS[i].name, ops_per_sec);
            fprintf(out, "%d,%s,%.0f\n", THREADS[t], LISTS[i].name, ops_per_sec);
        }
    }
    fclose(out);
    return 0;
}
