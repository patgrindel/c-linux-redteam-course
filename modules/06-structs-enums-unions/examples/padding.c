#include <stdio.h>
#include <stddef.h>   /* offsetof */

/* The compiler inserts PADDING bytes so each member starts on an address that
   is a multiple of its size (its "alignment"). Member ORDER therefore changes
   the struct's total size. You rarely need to care -- but you should know it
   happens, and that reordering largest-to-smallest usually packs tighter. */
struct bad {    /* char, int, char: padding wastes space */
    char a;
    int  b;
    char c;
};

struct good {   /* int, char, char: packs tighter */
    int  b;
    char a;
    char c;
};

int main(void) {
    printf("sizeof(struct bad)  = %zu\n", sizeof(struct bad));
    printf("sizeof(struct good) = %zu\n", sizeof(struct good));

    printf("struct bad : a@%zu b@%zu c@%zu\n",
           offsetof(struct bad, a),
           offsetof(struct bad, b),
           offsetof(struct bad, c));
    printf("struct good: b@%zu a@%zu c@%zu\n",
           offsetof(struct good, b),
           offsetof(struct good, a),
           offsetof(struct good, c));
    return 0;
}
