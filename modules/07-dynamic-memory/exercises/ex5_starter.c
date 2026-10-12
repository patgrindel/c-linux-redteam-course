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

void vec_free(IntVec *v) {
    free(v->data);
    v->data = NULL;
    v->len = 0;
    v->cap = 0;
}

/* TODO: remove the last element into *out; return false if empty. */
bool vec_pop(IntVec *v, int *out) {
    (void)v;
    (void)out;
    return false;
}

/* TODO: return the sum of all elements. */
long vec_sum(const IntVec *v) {
    (void)v;
    return 0;
}

int main(void) {
    IntVec v;
    vec_init(&v);
    vec_push(&v, 1);
    vec_push(&v, 2);
    vec_push(&v, 3);

    printf("sum = %ld\n", vec_sum(&v));   /* expect 6 */

    int x;
    while (vec_pop(&v, &x)) {
        printf("popped %d\n", x);          /* expect 3, 2, 1 */
    }
    printf("empty pop returns false\n");

    vec_free(&v);
    return 0;
}
