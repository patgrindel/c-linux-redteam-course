/* Exercise 4 — sum only the integer members of a tagged union.
   Always check the tag before touching a union member. */
#include <stdio.h>

typedef enum { VAL_INT, VAL_TEXT } ValueKind;

typedef struct {
    ValueKind kind;
    union {
        int         as_int;
        const char *as_text;
    } u;
} Value;

int sum_ints(const Value *vals, size_t n) {
    int total = 0;
    for (size_t i = 0; i < n; i++) {
        if (vals[i].kind == VAL_INT) {
            total += vals[i].u.as_int;   /* safe: tag says this member is live */
        }
    }
    return total;
}

int main(void) {
    Value vals[4] = {
        { .kind = VAL_INT,  .u.as_int = 10 },
        { .kind = VAL_TEXT, .u.as_text = "skip" },
        { .kind = VAL_INT,  .u.as_int = 5 },
        { .kind = VAL_INT,  .u.as_int = 7 },
    };
    size_t n = sizeof vals / sizeof vals[0];
    printf("sum of ints = %d\n", sum_ints(vals, n));   /* expect 22 */
    return 0;
}
