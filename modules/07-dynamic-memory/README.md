# Module 07 — Dynamic Memory & the Heap

> When you don't know the size until the program runs, you allocate on the heap.
> C hands you the keys *and* the responsibility: every `malloc` needs exactly one
> `free`, done by exactly one owner, at exactly the right time.

## Where we are

Module 04 showed the heap on the memory map; Module 06 built structs that often
need to outlive the function that creates them. Now we actually use the heap:
`malloc`, `calloc`, `realloc`, `free`. There is no garbage collector — this is the
single biggest difference from Python, and the source of C's most famous bugs. We'll
cause each bug on purpose, watch `valgrind` catch it, then build a reusable growable
array that feeds the capstone.

## Learning objectives

By the end you can:

- Allocate and release heap memory with `malloc`/`calloc`/`realloc`/`free`.
- Check every allocation for `NULL` and resize safely with the temp-pointer idiom.
- State clearly who **owns** a pointer and therefore who frees it.
- Recognize memory **leaks**, **use-after-free**, and **double-free**, and read how
  `valgrind` reports them.
- Build a growable dynamic array (a vector) with a clean init/push/free lifecycle.

---

## 1. Why the heap exists

Stack variables are automatic: created on entry to a function, destroyed on return,
with a size fixed at compile time. That's useless when:

- the size depends on runtime input (read N lines where N is unknown), or
- the data must **outlive** the function that created it (build a result and return
  a pointer to it).

For both, you ask the allocator for memory on the **heap**. It stays yours until you
`free` it — across function returns, for as long as you like. The price: *you* must
free it, once, or you leak (or worse).

> Python analogy: `list`, `dict`, and every object live on a heap too — but the
> interpreter frees them for you. In C, `free` is a line *you* write.

---

## 2. `malloc` and `free`: the basic cycle

`malloc(nbytes)` returns a pointer to an uninitialized block of at least `nbytes`,
or `NULL` on failure. `free(p)` returns that block to the allocator.

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    size_t n = 5;
    int *arr = malloc(n * sizeof *arr);   /* sizeof *arr == sizeof(int) */
    if (arr == NULL) {                    /* ALWAYS check */
        perror("malloc");
        return 1;
    }

    for (size_t i = 0; i < n; i++) {
        arr[i] = (int)(i * i);
    }
    for (size_t i = 0; i < n; i++) {
        printf("arr[%zu] = %d\n", i, arr[i]);
    }

    free(arr);
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/malloc_basics.c -o malloc_basics
$ ./malloc_basics
arr[0] = 0
arr[1] = 1
arr[2] = 4
arr[3] = 9
arr[4] = 16
```

Two habits to adopt immediately:

- **Size with `sizeof *ptr`, not `sizeof(int)`.** `malloc(n * sizeof *arr)` stays
  correct even if you later change `arr`'s type. Write the expression, not the type.
- **Check for `NULL` every time.** Allocation can fail. On failure, `malloc` returns
  `NULL` and dereferencing it crashes (Module 04).

`malloc` leaves the bytes **uninitialized** — reading them before you write is a bug.

---

## 3. `calloc`: allocate and zero

`calloc(count, size)` allocates `count * size` bytes and sets them all to zero. Use
it when a zeroed start is what you want (counters, flags, structs), and because it
also checks `count * size` for overflow.

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    size_t n = 5;
    int *arr = calloc(n, sizeof *arr);
    if (arr == NULL) {
        perror("calloc");
        return 1;
    }
    for (size_t i = 0; i < n; i++) {
        printf("arr[%zu] = %d\n", i, arr[i]);   /* all zero */
    }
    free(arr);
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/calloc_demo.c -o calloc_demo
$ ./calloc_demo
arr[0] = 0
arr[1] = 0
arr[2] = 0
arr[3] = 0
arr[4] = 0
```

---

## 4. `realloc`: resize, and the temp-pointer rule

`realloc(p, newsize)` grows or shrinks a block, preserving the old contents and
possibly **moving** it to a new address. It returns the new pointer, or `NULL` on
failure. The trap: on failure the *old* block is still valid, so if you wrote
`p = realloc(p, ...)` and it failed, you'd overwrite your only pointer with `NULL`
and leak the old block. Always go through a temp:

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    size_t n = 2;
    int *arr = malloc(n * sizeof *arr);
    if (arr == NULL) { perror("malloc"); return 1; }
    arr[0] = 1;
    arr[1] = 2;

    size_t new_n = 4;
    int *tmp = realloc(arr, new_n * sizeof *arr);
    if (tmp == NULL) {
        perror("realloc");
        free(arr);          /* old block still ours */
        return 1;
    }
    arr = tmp;              /* only now overwrite arr */
    arr[2] = 3;
    arr[3] = 4;

    for (size_t i = 0; i < new_n; i++) {
        printf("arr[%zu] = %d\n", i, arr[i]);
    }
    free(arr);
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/realloc_demo.c -o realloc_demo
$ ./realloc_demo
arr[0] = 1
arr[1] = 2
arr[2] = 3
arr[3] = 4
```

Because `realloc` may move the block, **any other pointer into the old block is
invalid afterward**. Re-derive pointers from the new base.

---

## 5. Ownership: who frees what

The hardest part of C memory isn't the API — it's deciding, for each pointer, *who
is responsible for freeing it, and when*. That contract is called **ownership**.
Write it down in a comment at every function that allocates or frees.

A function that returns heap memory transfers ownership to its caller:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Returns a NEW heap copy of s. The CALLER owns it and must free it. */
char *dup_string(const char *s) {
    size_t len = strlen(s);
    char *copy = malloc(len + 1);      /* +1 for the NUL */
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, s, len + 1);          /* copy the bytes and the NUL */
    return copy;
}

int main(void) {
    char *greeting = dup_string("hello, heap");
    if (greeting == NULL) {
        perror("dup_string");
        return 1;
    }
    printf("%s\n", greeting);
    free(greeting);     /* caller frees */
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/ownership.c -o ownership
$ ./ownership
hello, heap
```

Simple rules that prevent most bugs:

- **One owner.** Exactly one piece of code is responsible for freeing a block.
- **Free once.** Each `malloc`/`calloc`/`realloc` pairs with exactly one `free`.
- **NULL after free.** Setting `p = NULL` after `free(p)` makes a later accidental
  free harmless (`free(NULL)` does nothing) and turns a dangling read into a clean
  NULL-deref crash you'll spot fast.

---

## 6. The three classic bugs — and how `valgrind` finds them

`valgrind ./prog` runs your program under instrumentation and reports memory
errors with line numbers. Install it (`sudo apt install valgrind`) and make it a
reflex. Here are the three bugs you must recognize.

### Leak: allocated, never freed

```c
int *data = malloc(100 * sizeof *data);  /* ... */ return 0;   /* no free */
```

(`examples/leak_demo.c`) The program runs fine; the leak is invisible without a
tool. Under valgrind:

```console
$ gcc -std=c11 -Wall -Wextra -g examples/leak_demo.c -o leak_demo
$ valgrind --leak-check=full ./leak_demo
==12345== HEAP SUMMARY:
==12345==     in use at exit: 400 bytes in 1 blocks
==12345==   total heap usage: 1 allocs, 0 frees, 400 bytes allocated
==12345==
==12345== 400 bytes in 1 blocks are definitely lost in loss record 1 of 1
==12345==    at 0x4848899: malloc (vg_replace_malloc.c:...)
==12345==    by 0x109165: main (leak_demo.c:11)
==12345==
==12345== LEAK SUMMARY:
==12345==    definitely lost: 400 bytes in 1 blocks
```

"definitely lost: 400 bytes" with the `malloc` call site (`leak_demo.c:11`) is the
tell. Compile with `-g` so valgrind can show line numbers.

### Use-after-free: touching freed memory

```c
destroy(msg);          /* frees msg */
printf("%s\n", msg);   /* msg now dangles */
```

(`examples/use_after_free.c`)

```console
$ gcc -std=c11 -Wall -Wextra -g examples/use_after_free.c -o use_after_free
$ valgrind ./use_after_free
==12360== Invalid read of size 1
==12360==    at 0x48C3A0: strlen (vg_replace_strmem.c:...)
==12360==    by 0x109210: main (use_after_free.c:21)
==12360==  Address 0x4a8c040 is 0 bytes inside a block of size 16 free'd
==12360==    at 0x4849A0C: free (vg_replace_malloc.c:...)
==12360==    by 0x1091C6: destroy (use_after_free.c:11)
```

valgrind shows both the bad read *and* where the block was freed — exactly the
information you need. Without a tool this bug is maddening, because it often
"works" by luck.

### Double-free: freeing twice

```c
destroy(data);
destroy(data);   /* second free of the same block */
```

(`examples/double_free.c`)

```console
$ gcc -std=c11 -Wall -Wextra -g examples/double_free.c -o double_free
$ valgrind ./double_free
==12377== Invalid free() / delete / delete[] / realloc()
==12377==    at 0x4849A0C: free (vg_replace_malloc.c:...)
==12377==    by 0x1091D6: destroy (double_free.c:9)
==12377==  Address 0x4a8c040 is 0 bytes inside a block of size 4 free'd
==12377==    at 0x4849A0C: free (vg_replace_malloc.c:...)
==12377==    by 0x1091D6: destroy (double_free.c:9)
```

A clean run (no errors, everything freed) ends with:

```console
==12399== All heap blocks were freed -- no leaks are possible
==12399== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

That "0 errors" line is the goal for every program you write from here on.

---

## 7. Worked example: a growable dynamic array (vector)

Arrays are fixed size; real programs collect an *unknown* number of items. The
standard solution is a **vector**: a struct that owns a heap buffer and doubles its
capacity when full. This is the structure the capstone uses to gather findings, so
study it closely.

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

    vec_free(&v);
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra -g examples/int_vector.c -o int_vector
$ ./int_vector
pushed  0 -> len=1 cap=4
pushed  1 -> len=2 cap=4
pushed  4 -> len=3 cap=4
pushed  9 -> len=4 cap=4
pushed 16 -> len=5 cap=8
pushed 25 -> len=6 cap=8
pushed 36 -> len=7 cap=8
pushed 49 -> len=8 cap=8
pushed 64 -> len=9 cap=16
pushed 81 -> len=10 cap=16
contents: 0 1 4 9 16 25 36 49 64 81
$ valgrind ./int_vector
...
==12410== All heap blocks were freed -- no leaks are possible
==12410== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

Notice the capacity jumps `4 → 8 → 16` instead of growing by one each time.
Doubling makes `vec_push` *amortized O(1)*: most pushes just store a value; only
the occasional one reallocates. The lifecycle is deliberate and symmetric:
`vec_init` sets a safe empty state, `vec_push` grows it, and `vec_free` releases the
buffer and resets the struct so it can't dangle or double-free. One owner (`main`),
one `vec_free`. That discipline — init, use, free — scales to every data structure
you'll build.

---

## Common mistakes / gotchas

- **Not checking `NULL`.** Every allocation can fail. Check, then use.
- **Leaking.** Every successful `malloc`/`calloc`/`realloc` needs one `free`. Early
  `return`s and error paths are where leaks hide — free before you bail.
- **Use-after-free / dangling pointers.** Don't touch memory after `free`. Set the
  pointer to `NULL` so misuse fails loudly.
- **Double free.** Free once, from one owner. `NULL`-after-free makes a stray second
  `free` harmless.
- **`realloc` straight onto the same pointer.** Use a temp; a failed `realloc`
  returns `NULL` without freeing the old block.
- **Reading uninitialized `malloc` memory.** Those bytes are garbage until you write
  them. Use `calloc` if you need zeros.
- **Freeing something not from `malloc`.** Never `free` a stack address, a string
  literal, or a pointer into the middle of a block. Free only what an allocator gave
  you, at the base address it returned.

## Recap

- The heap gives you memory whose size is decided at runtime and whose lifetime you
  control. There's no GC — you free it.
- `malloc` (uninitialized), `calloc` (zeroed), `realloc` (resize, use a temp),
  `free` (once). Check every allocation for `NULL`.
- Ownership is a contract: one owner frees once, at the right time. Document it.
- Leaks, use-after-free, and double-free are the classic bugs; `valgrind` pinpoints
  each with line numbers — aim for "0 errors."
- A vector (owning struct + buffer that doubles) is the reusable growable array;
  init / push / free is the lifecycle you'll reuse everywhere.

## Exercises

See [`exercises/README.md`](exercises/README.md). Build with
`gcc -std=c11 -Wall -Wextra -g` and check each with `valgrind`.

## Further reading

- `man 3 malloc`, `man 3 realloc`, `man 3 free`, `man 3 calloc`
- `man 1 valgrind`; the Memcheck manual (valgrind.org).
- Later in the course: `-fsanitize=address` (ASan) catches many of these at runtime
  with less overhead than valgrind (Module 11).
