# Module 06 — Structs, Enums & Unions

> How to model structured data in C. A struct is a record; an enum names a set of
> constants; a union lets several fields share one slot. These are the building
> blocks of every real C program's data.

## Where we are

You can now handle scalars, pointers, arrays, and strings. Real programs need to
group related values into one thing — a "host" with an IP, a port, and a service;
a "finding" with a severity and a message. Python gives you classes and tuples; C
gives you `struct`, `enum`, and `union`. This module is the data-modeling toolkit
you'll lean on for the capstone.

## Learning objectives

By the end you can:

- Define a `struct`, set and read members with `.`, and through a pointer with `->`.
- Use `typedef` to give a struct a clean one-word name.
- Build nested structs and arrays of structs.
- Use `enum` for readable named constants and drive a `switch` with it.
- Explain what a `union` is and use a **tagged union** safely.
- Explain, at an awareness level, why `struct` sizes include **padding**.

---

## 1. `struct`: grouping related values

A `struct` is a record: several named fields stored together. Here is a 2-D point.

```c
#include <stdio.h>

struct point {
    int x;
    int y;
};

int main(void) {
    struct point p;        /* the type name is "struct point" */
    p.x = 3;               /* the dot operator reaches a member */
    p.y = 4;
    printf("p = (%d, %d)\n", p.x, p.y);

    struct point q = {10, 20};            /* fields in declared order */
    printf("q = (%d, %d)\n", q.x, q.y);

    struct point r = {.x = 7, .y = 8};    /* designated initializers: clearer */
    printf("r = (%d, %d)\n", r.x, r.y);
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/struct_basics.c -o struct_basics
$ ./struct_basics
p = (3, 4)
q = (10, 20)
r = (7, 8)
```

Note the type is **`struct point`**, two words. (We fix that with `typedef` below.)
A struct is copied by value on assignment and when passed to a function — unlike a
Python object, which is passed by reference.

---

## 2. `.` vs `->`: members through a value vs. through a pointer

When you hold a struct *by value*, use `.`. When you hold a *pointer to* a struct,
use `->`, which is just shorthand for "dereference, then dot."

```c
#include <stdio.h>

struct point {
    int x;
    int y;
};

void move(struct point *p, int dx, int dy) {
    p->x += dx;     /* p->x means (*p).x */
    p->y += dy;
}

int main(void) {
    struct point p = {0, 0};
    move(&p, 3, 4);
    printf("p = (%d, %d)\n", p.x, p.y);
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/struct_pointer.c -o struct_pointer
$ ./struct_pointer
p = (3, 4)
```

Passing `&p` lets `move` edit the caller's struct (output-parameter idiom from
Module 04). It also avoids copying — important once structs get large. **Rule of
thumb: pass big structs by pointer**, often `const` if you won't modify them.

---

## 3. `typedef`: a clean name

Writing `struct point` everywhere is noisy. `typedef` makes an alias. The common
idiom combines an anonymous struct with a typedef:

```c
#include <stdio.h>

typedef struct {
    int x;
    int y;
} Point;

Point add(Point a, Point b) {
    Point sum = {a.x + b.x, a.y + b.y};
    return sum;    /* small structs returned by value -- fine */
}

int main(void) {
    Point a = {1, 2};
    Point b = {3, 4};
    Point c = add(a, b);
    printf("c = (%d, %d)\n", c.x, c.y);
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/typedef_struct.c -o typedef_struct
$ ./typedef_struct
c = (4, 6)
```

Now the type is just `Point`. (Convention in this course: `CamelCase` for typedef'd
types, `snake_case` for variables and functions.) Returning a small struct by value
is perfectly fine — the whole struct is copied back.

---

## 4. Nested structs and arrays of structs

Structs compose. A rectangle is two points:

```c
#include <stdio.h>

typedef struct { int x; int y; } Point;
typedef struct { Point top_left; Point bottom_right; } Rect;

int area(const Rect *r) {
    int w = r->bottom_right.x - r->top_left.x;
    int h = r->bottom_right.y - r->top_left.y;
    return w * h;
}

int main(void) {
    Rect box = { {0, 0}, {4, 3} };   /* braces nest like the struct */
    printf("area = %d\n", area(&box));
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/nested_struct.c -o nested_struct
$ ./nested_struct
top-left     = (0, 0)
bottom-right = (4, 3)
area = 12
```

And an **array of structs** is the natural "table of records." Here, scan findings
on a host you're assessing:

```c
#include <stdio.h>

typedef struct {
    const char *ip;
    int         port;
    const char *service;
} Finding;

int main(void) {
    Finding findings[3] = {
        {"10.0.0.5",  22,  "ssh"},
        {"10.0.0.5",  80,  "http"},
        {"10.0.0.9",  445, "smb"},
    };
    size_t n = sizeof findings / sizeof findings[0];

    for (size_t i = 0; i < n; i++) {
        printf("%-10s port %-4d %s\n",
               findings[i].ip, findings[i].port, findings[i].service);
    }
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/array_of_structs.c -o array_of_structs
$ ./array_of_structs
10.0.0.5   port 22   ssh
10.0.0.5   port 80   http
10.0.0.9   port 445  smb
```

This is exactly the shape the capstone uses to collect results before printing a
report.

---

## 5. `enum`: named constants

An `enum` defines named integer constants. They beat "magic numbers" because the
name documents intent, and the compiler can check a `switch` covers them.

```c
#include <stdio.h>

typedef enum {
    SEV_INFO,       /* 0 */
    SEV_LOW,        /* 1 */
    SEV_MEDIUM,     /* 2 */
    SEV_HIGH,       /* 3 */
    SEV_CRITICAL    /* 4 */
} Severity;

const char *severity_name(Severity s) {
    switch (s) {
        case SEV_INFO:     return "INFO";
        case SEV_LOW:      return "LOW";
        case SEV_MEDIUM:   return "MEDIUM";
        case SEV_HIGH:     return "HIGH";
        case SEV_CRITICAL: return "CRITICAL";
    }
    return "UNKNOWN";
}

int main(void) {
    Severity finding = SEV_HIGH;
    printf("severity %d = %s\n", finding, severity_name(finding));
    for (Severity s = SEV_INFO; s <= SEV_CRITICAL; s++) {
        printf("  %d -> %s\n", s, severity_name(s));
    }
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/enum_demo.c -o enum_demo
$ ./enum_demo
severity 3 = HIGH
  0 -> INFO
  1 -> LOW
  2 -> MEDIUM
  3 -> HIGH
  4 -> CRITICAL
```

Values count from 0 by default; you can assign explicit values
(`enum { A = 1, B = 2, C = 4 }`) when they matter, e.g. for bit flags. Under the
hood an enum value is just an `int`, so it prints with `%d`.

---

## 6. `union`: one slot, several interpretations

A `union` looks like a struct but its members **overlap the same memory** — only
one is valid at a time. A union is only as large as its biggest member. The catch:
the union itself doesn't record *which* member is live, so you must track that with
a companion "tag." The struct-with-a-tag pattern is a **tagged union** (a.k.a.
discriminated union / variant).

```c
#include <stdio.h>

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
```

```console
$ gcc -std=c11 -Wall -Wextra examples/union_demo.c -o union_demo
$ ./union_demo
int    42
double 3.14
text   hello
sizeof(Value) = 16 (tag + the LARGEST member, plus padding)
```

**When is a union useful?** When a value is "one of several kinds" and you want to
store it compactly: a config entry that is an int *or* a string, a token that is a
number *or* an identifier, a protocol message with a type byte and a payload. The
discipline: always read the member that the tag says is valid. Reading the wrong one
is a bug C will not catch for you.

---

## 7. Padding and alignment (awareness level)

A `struct` is usually **bigger than the sum of its fields** because the compiler
inserts *padding* so each member begins at an address that's a multiple of its size
(its *alignment*). You rarely need to act on this, but you should recognize it.

```c
#include <stdio.h>
#include <stddef.h>   /* offsetof */

struct bad  { char a; int b; char c; };   /* padding wastes space */
struct good { int b; char a; char c; };   /* packs tighter */

int main(void) {
    printf("sizeof(struct bad)  = %zu\n", sizeof(struct bad));
    printf("sizeof(struct good) = %zu\n", sizeof(struct good));
    printf("struct bad : a@%zu b@%zu c@%zu\n",
           offsetof(struct bad, a), offsetof(struct bad, b), offsetof(struct bad, c));
    printf("struct good: b@%zu a@%zu c@%zu\n",
           offsetof(struct good, b), offsetof(struct good, a), offsetof(struct good, c));
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/padding.c -o padding
$ ./padding
sizeof(struct bad)  = 12
sizeof(struct good) = 8
struct bad : a@0 b@4 c@8
struct good: b@0 a@4 c@5
```

Same three fields, different size. In `struct bad`, `char a` at offset 0 is followed
by 3 padding bytes so `int b` can land on a 4-byte boundary at offset 4; then `char c`
needs trailing padding so arrays of the struct stay aligned. In `struct good`, the
`int` leads and the two `char`s tuck in behind it.

Takeaways at this level:
- `sizeof(struct)` ≥ sum of members; the difference is padding.
- Don't assume a struct's layout is "tightly packed" when writing it to a file or a
  socket — that's a portability bug (we'll come back to it with I/O).
- As a rough habit, order members **largest type first** to minimize padding.

---

## Common mistakes / gotchas

- **`.` vs `->`.** Dot for a struct value, arrow for a pointer to a struct. Using
  the wrong one is a compile error — read it as a reminder, not an insult.
- **Forgetting the tag with a union.** The union won't tell you which member is
  valid. Store and check a tag; never read a member you didn't write.
- **Assuming struct size = sum of fields.** Padding makes it bigger. Use `sizeof`,
  never hand-count.
- **Comparing structs with `==`.** Not allowed in C. Compare field by field (or
  `memcmp` only when you know there's no padding to differ — usually safer not to).
- **Returning a pointer to a local struct.** Same dangling-pointer trap as Module 04.
  Return by value, or allocate on the heap (Module 07).
- **`struct point` vs `Point`.** Without a `typedef`, the type name keeps the
  `struct` keyword. Pick the typedef'd form for readability.

## Recap

- `struct` groups named fields; access with `.`, or `->` through a pointer.
- `typedef` gives a struct a one-word name; this course uses `CamelCase` for them.
- Structs nest, and arrays of structs are the go-to "table of records."
- `enum` names integer constants and pairs naturally with `switch`.
- `union` overlaps members to save space; wrap it in a struct with a tag to use it
  safely (a tagged union).
- Structs carry padding, so `sizeof` ≥ the sum of fields; order members big-to-small
  to pack tighter.

## Exercises

See [`exercises/README.md`](exercises/README.md). Build with
`gcc -std=c11 -Wall -Wextra`.

## Further reading

- `man 3 offsetof`
- K&R Chapter 6 (Structures).
- "The Lost Art of Structure Packing" (Eric S. Raymond) — a friendly deep dive on
  padding when you're ready.
