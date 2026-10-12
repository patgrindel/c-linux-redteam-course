#include <stdio.h>

/* A union stores exactly ONE of its members at a time; all members overlap the
   same bytes, so the union is only as big as its largest member. Because the
   union itself does not remember which member is live, you pair it with a "tag"
   -- this struct-with-tag-and-union is called a TAGGED UNION. */
typedef enum { VAL_INT, VAL_DOUBLE, VAL_TEXT } ValueKind;

typedef struct {
    ValueKind kind;
    union {
        int         as_int;
        double      as_double;
        const char *as_text;
    } u;
} Value;

void print_value(const Value *v) {
    switch (v->kind) {
        case VAL_INT:    printf("int    %d\n", v->u.as_int);    break;
        case VAL_DOUBLE: printf("double %g\n", v->u.as_double); break;
        case VAL_TEXT:   printf("text   %s\n", v->u.as_text);   break;
    }
}

int main(void) {
    Value a = { .kind = VAL_INT,    .u.as_int = 42 };
    Value b = { .kind = VAL_DOUBLE, .u.as_double = 3.14 };
    Value c = { .kind = VAL_TEXT,   .u.as_text = "hello" };

    print_value(&a);
    print_value(&b);
    print_value(&c);

    printf("sizeof(Value) = %zu (tag + the LARGEST member, plus padding)\n",
           sizeof(Value));
    return 0;
}
