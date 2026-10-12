/* Exercise 5 — extend the vector with pop and sum.
   vec_pop only decrements len (the buffer keeps its capacity, which is cheap
   and avoids churn); it guards against popping an empty vector. vec_sum uses a
   long accumulator so a long run of pushes is less likely to overflow. */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int   *data;
    size_t len;
    size_t cap;
} IntVec;

void vec_init(IntVec *v) {
    v->data = NULL;
    v->len = 0;
    v->cap = 0;
}

bool vec_push(IntVec *v, int value) {
    if (v->len == v->cap) {
        size_t new_cap = (v->cap == 0) ? 4 : v->cap * 2;
        int *tmp = realloc(v->data, new_cap * sizeof *v->data);
        if (tmp == NULL) {
            return false;
        }
        v->data = tmp;
        v->cap = new_cap;
    }
    v->data[v->len++] = value;
    return true;
}

bool vec_pop(IntVec *v, int *out) {
    if (v->len == 0) {
        return false;            /* nothing to pop */
    }
    *out = v->data[--v->len];    /* step len back, then read */
    return true;
}

long vec_sum(const IntVec *v) {
    long total = 0;
    for (size_t i = 0; i < v->len; i++) {
        total += v->data[i];
    }
    return total;
}

void vec_free(IntVec *v) {
    free(v->data);
    v->data = NULL;
    v->len = 0;
    v->cap = 0;
}

int main(void) {
    IntVec v;
    vec_init(&v);
    vec_push(&v, 1);
    vec_push(&v, 2);
    vec_push(&v, 3);

    printf("sum = %ld\n", vec_sum(&v));   /* 6 */

    int x;
    while (vec_pop(&v, &x)) {
        printf("popped %d\n", x);          /* 3, 2, 1 */
    }
    printf("empty pop returns false\n");

    vec_free(&v);
    return 0;
}
