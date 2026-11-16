#ifndef LISTS_H
#define LISTS_H

void *gl_new(void);
void gl_free(void *l);
int gl_find(void *l, int k);
int gl_insert(void *l, int k);
int gl_delete(void *l, int k);

void *hoh_new(void);
void hoh_free(void *l);
int hoh_find(void *l, int k);
int hoh_insert(void *l, int k);
int hoh_delete(void *l, int k);

#endif
