#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* A growable array ("vector") of ints. It owns a heap buffer and DOUBLES its
   capacity whenever it fills up -- the same trick Python's list uses. This is
   the data structure the capstone uses to gather an unknown number of findings. */
typedef struct {
    int   *data;   /* heap buffer, or NULL while empty */
    size_t len;    /* elements currently stored */
    size_t cap;    /* elements the buffer can hold */
} IntVec;

void vec_init(IntVec *v) {
    v->data = NULL;
    v->len = 0;
    v->cap = 0;
}

/* Append one value. Returns true on success; on allocation failure returns
   false and leaves the vector unchanged. */
bool vec_push(IntVec *v, int value) {
    if (v->len == v->cap) {
        size_t new_cap = (v->cap == 0) ? 4 : v->cap * 2;
        int *tmp = realloc(v->data, new_cap * sizeof *v->data);
        if (tmp == NULL) {
            return false;          /* old buffer still valid */
        }
        v->data = tmp;
        v->cap = new_cap;
    }
    v->data[v->len++] = value;
    return true;
}

int vec_get(const IntVec *v, size_t i) {
    return v->data[i];
}

/* Release the buffer and reset to a safe empty state, so an accidental second
   call (or use) does not double-free or dangle. */
void vec_free(IntVec *v) {
    free(v->data);
    v->data = NULL;
    v->len = 0;
    v->cap = 0;
}

int main(void) {
    IntVec v;
    vec_init(&v);

    for (int i = 0; i < 10; i++) {
        if (!vec_push(&v, i * i)) {
            fprintf(stderr, "out of memory\n");
            vec_free(&v);
            return 1;
        }
        printf("pushed %2d -> len=%zu cap=%zu\n", i * i, v.len, v.cap);
    }

    printf("contents:");
    for (size_t i = 0; i < v.len; i++) {
        printf(" %d", vec_get(&v, i));
    }
    putchar('\n');

    vec_free(&v);     /* one owner, one free */
    return 0;
}
