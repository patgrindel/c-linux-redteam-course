# Module 04 — Pointers & the Memory Model

> The one topic that makes or breaks C. A pointer is just a variable that holds
> a memory address. Once that clicks, the rest of C stops being mysterious.

## Where we are

In Module 03 you wrote functions and learned that C passes arguments **by value** —
the function gets *copies*. That raised a question we dodged: how does a function
change something back in the caller? The answer is pointers, and they are the key
to the whole language. This is a 3-day module. Go slow. Type every example.

## Learning objectives

By the end you can:

- Explain what a pointer *is* in terms of addresses and memory cells.
- Use `&` (address-of) and `*` (dereference) correctly, and say which is which.
- Draw the memory layout of a running process (stack, heap, static, text) and say
  where any given variable lives.
- Do pointer arithmetic and explain why `p + 1` moves by one *element*.
- Recognize `NULL`, why dereferencing it crashes, and how to guard against it.
- Use `const` to mark pointers to read-only data.
- Write functions that return results through **output parameters** (`swap(int*, int*)`).
- Read and use a basic **function pointer**.

---

## 1. The mental model: memory is a giant array of bytes

Think of your program's memory as one enormous array of bytes. Every byte has an
**address** — an integer index, usually written in hex like `0x7ffe1a2b3c40`. A
variable is just a *name* the compiler gives to one or more of those bytes.

In Python you never see addresses; the interpreter hides them. In C they are right
there, and a **pointer** is a variable whose *value is an address*.

Two operators are the whole game:

| Operator | Name | Reads as | Gives you |
|----------|------|----------|-----------|
| `&x`     | address-of | "address of x" | the address where `x` lives |
| `*p`     | dereference | "the thing p points at" | the value stored at that address |

They are inverses: `*(&x)` is just `x`.

### Picture it

Say `int x = 42;` landed at address `0x7ffe...40`, and `int *p = &x;` landed at
`0x7ffe...48`. In memory:

```
        +-------------------+
0x..40  |        42         |   <- x  (an int)
        +-------------------+
0x..48  |    0x7ffe...40    |   <- p  (a pointer; its VALUE is the address of x)
        +-------------------+
                 |
                 +--- *p follows this arrow back to the 42
```

`p` does not contain 42. It contains the *address* of the box that contains 42.
`*p` means "go to that address and look."

### Example: `&` and `sizeof`

```c
#include <stdio.h>

int main(void) {
    int x = 42;
    int *p = &x;    /* p now holds the address of x */

    printf("value of x       : %d\n", x);
    printf("address of x     : %p\n", (void *)&x);
    printf("value stored in p: %p\n", (void *)p);
    printf("size of an int   : %zu bytes\n", sizeof x);
    printf("size of p (int *): %zu bytes\n", sizeof p);
    return 0;
}
```

Build and run (`examples/address_of.c`):

```console
$ gcc -std=c11 -Wall -Wextra examples/address_of.c -o address_of
$ ./address_of
value of x       : 42
address of x     : 0x7ffe1a2b3c40
value stored in p: 0x7ffe1a2b3c40
size of an int   : 4 bytes
size of p (int *): 8 bytes
```

The two addresses match: `p`'s value *is* the address of `x`. Your exact addresses
will differ every run (the OS randomizes them) — that's fine.

**Notes on the printf:**
- `%p` prints a pointer. Always cast the argument to `(void *)` — that is what `%p`
  officially expects, and it silences a warning.
- `%zu` is the specifier for `size_t`, the unsigned type `sizeof` returns. Using
  `%d` here would be wrong on 64-bit systems.
- A pointer is **8 bytes on a 64-bit machine** regardless of what it points at — it
  is just an address. `int *`, `char *`, `double *` are all 8 bytes.

---

## 2. Dereferencing: reading and writing through a pointer

`*p` on the right of `=` *reads* the pointed-to value. `*p` on the left *writes* it.

```c
#include <stdio.h>

int main(void) {
    int x = 42;
    int *p = &x;

    printf("x via x : %d\n", x);
    printf("x via *p: %d\n", *p);   /* read through the pointer */

    *p = 99;                        /* write through the pointer */
    printf("after *p = 99, x = %d\n", x);
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/deref.c -o deref
$ ./deref
x via x : 42
x via *p: 42
after *p = 99, x = 99
```

Writing `*p = 99` changed `x` itself, because `p` points *at* `x`. There is no copy.
This is the mechanism behind everything else in this module.

> **Star means two different things.** In a *declaration*, `int *p;` the `*` says
> "p is a pointer." In an *expression*, `*p` the `*` means "dereference." Same
> symbol, different jobs. Read declarations as "p is a pointer to int."

---

## 3. Where variables live: the process memory map

A running C program carves its memory into regions. You don't need every detail,
but you must know **which region a variable lives in**, because that decides its
*lifetime* (how long it stays valid).

```
   high addresses
  +---------------------------+
  |   command-line args/env   |
  +---------------------------+
  |         STACK             |   local variables, function call frames
  |            |              |   grows DOWNWARD as you call functions
  |            v              |
  |                           |
  |            ^              |
  |            |              |
  |          HEAP             |   malloc/calloc memory (Module 07)
  +---------------------------+   grows UPWARD as you allocate
  |   BSS  (uninit globals)   |   zero-initialized static data
  +---------------------------+
  |   DATA (init globals)     |   static data with an initial value
  +---------------------------+
  |   TEXT (code)             |   your compiled instructions (read-only)
  +---------------------------+
   low addresses
```

- **Stack**: locals and parameters. Automatically created when a function is called
  and destroyed when it returns. Fast. Fixed, limited size.
- **Heap**: memory you request at runtime with `malloc` and must return with `free`
  (Module 07). It lives until *you* free it.
- **Data / BSS**: globals and `static` variables. Live for the whole program.
- **Text**: the machine code of your functions. Read-only.

### See it for yourself

```c
#include <stdio.h>
#include <stdlib.h>

int global_initialized = 1;   /* data segment */
int global_uninitialized;     /* bss  segment */

static void a_function(void) { }

int main(void) {
    int local = 0;                        /* stack */
    int *heap = malloc(sizeof *heap);     /* heap  */
    if (heap == NULL) {
        perror("malloc");
        return 1;
    }
    *heap = 7;

    printf("text   (code)   a_function          : %p\n", (void *)a_function);
    printf("data   (init)   global_initialized  : %p\n", (void *)&global_initialized);
    printf("bss    (uninit) global_uninitialized: %p\n", (void *)&global_uninitialized);
    printf("heap            *heap               : %p\n", (void *)heap);
    printf("stack           local               : %p\n", (void *)&local);

    free(heap);
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/memory_layout.c -o memory_layout
$ ./memory_layout
text   (code)   a_function          : 0x5612a4c0a169
data   (init)   global_initialized  : 0x5612a4c0d010
bss    (uninit) global_uninitialized: 0x5612a4c0d018
heap            *heap               : 0x5612a5f0b2a0
stack           local               : 0x7ffc3d8f4a1c
```

Exact numbers vary, but the *shape* holds: code lowest, then globals, then heap,
with the stack far away up high. The single most important takeaway:

> **A local variable's address is only valid while its function is still running.**
> Return a pointer to a local and you get a *dangling pointer* — the box is gone.
> (The compiler warns about the obvious case; we'll handle the heap way in Module 07.)

---

## 4. Pointer arithmetic: `+ 1` means "next element"

If `p` points at an `int`, then `p + 1` does **not** add 1 byte — it adds
`sizeof(int)` bytes, so it points at the *next int*. The compiler scales for you.
This is why pointers have types: the type says how big a step is.

```c
#include <stdio.h>

int main(void) {
    int nums[5] = {10, 20, 30, 40, 50};
    int *p = nums;   /* an array name used as a value decays to &nums[0] */

    for (size_t i = 0; i < 5; i++) {
        printf("p + %zu  ->  address %p  ->  value %d\n",
               i, (void *)(p + i), *(p + i));
    }

    printf("\nnums[2] is %d, and *(nums + 2) is %d  (same thing)\n",
           nums[2], *(nums + 2));
    printf("(p + 1) - p = %td element\n", (p + 1) - p);
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/pointer_arith.c -o pointer_arith
$ ./pointer_arith
p + 0  ->  address 0x7ffd0c1e2a10  ->  value 10
p + 1  ->  address 0x7ffd0c1e2a14  ->  value 20
p + 2  ->  address 0x7ffd0c1e2a18  ->  value 30
p + 3  ->  address 0x7ffd0c1e2a1c  ->  value 40
p + 4  ->  address 0x7ffd0c1e2a20  ->  value 50
nums[2] is 30, and *(nums + 2) is 30  (same thing)
(p + 1) - p = 1 element
```

Look at the addresses: each is **4 larger** than the last, because an `int` is 4
bytes. And notice `nums[2]` is *literally defined* as `*(nums + 2)`. That is the
big reveal of the next module: **indexing is pointer arithmetic**. We'll go deep on
arrays in Module 05.

Subtracting two pointers into the same array gives the number of elements between
them (a `ptrdiff_t`, printed with `%td`).

---

## 5. `NULL` and the crash you must be able to cause on purpose

`NULL` is a special pointer value meaning "points at nothing." Use it to say "no
object here yet." Dereferencing `NULL` is a bug the hardware and OS catch: your
program receives a **segmentation fault** (SIGSEGV) and dies immediately.

You should be able to *make this happen on purpose* so you recognize it later.

```c
#include <stdio.h>

int main(void) {
    int *p = NULL;

    printf("p holds the address: %p\n", (void *)p);
    printf("about to dereference NULL...\n");
    fflush(stdout);         /* force the message out before we crash */

    int value = *p;         /* CRASH: reading through a NULL pointer */
    printf("you will never see this line: %d\n", value);
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/null_deref.c -o null_deref
$ ./null_deref
p holds the address: (nil)
about to dereference NULL...
Segmentation fault (core dumped)
```

It compiled with zero warnings and crashed at runtime — the defining experience of
C. The fix is always the same shape: **check before you dereference.**

```c
if (p != NULL) {
    /* safe to use *p here */
}
```

(We call `fflush(stdout)` because `printf` buffers output; without it the crash can
happen before the buffered text reaches your terminal, making the crash look like it
came one line earlier.)

---

## 6. `const` correctness: promise not to write

A function that only *reads* through a pointer should say so with `const`. It
documents intent and lets the compiler reject accidental writes.

```c
#include <stdio.h>

size_t count_spaces(const char *s) {
    size_t n = 0;
    while (*s != '\0') {
        if (*s == ' ') {
            n++;
        }
        s++;            /* allowed: we move the pointer, not the data */
        /* *s = 'x';     NOT allowed: would fail to compile */
    }
    return n;
}

int main(void) {
    const char *msg = "red team go";
    printf("\"%s\" has %zu spaces\n", msg, count_spaces(msg));
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/const_pointers.c -o const_pointers
$ ./const_pointers
"red team go" has 2 spaces
```

Read it right-to-left: `const char *s` is "a pointer to `const char`" — the
*characters* are read-only, but the pointer variable `s` itself can still move
(`s++`). If you wanted the opposite (pointer can't move, data can) you'd write
`char * const s`. For now, the useful habit is: **mark input pointers you don't
intend to modify as `const`.**

---

## 7. Output parameters: the real reason we need `&`

A C function returns exactly one value. When you need to send *several* results
back, or a result plus a success flag, you pass **pointers to the caller's
variables** and write through them. This is the C idiom you'll use constantly.

The textbook example is `swap`, which cannot work by value:

```c
#include <stdio.h>

/* WRONG: parameters are COPIES. Swaps the copies, then throws them away. */
void swap_by_value(int a, int b) {
    int tmp = a;
    a = b;
    b = tmp;
}

/* RIGHT: pass the ADDRESSES so we can edit the caller's actual variables. */
void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int main(void) {
    int x = 1, y = 2;

    swap_by_value(x, y);
    printf("after swap_by_value: x = %d, y = %d  (unchanged!)\n", x, y);

    swap(&x, &y);
    printf("after swap         : x = %d, y = %d  (swapped)\n", x, y);
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/swap.c -o swap
$ ./swap
after swap_by_value: x = 1, y = 2  (unchanged!)
after swap         : x = 1, y = 2  (swapped)
```

This is exactly the "multiple return values via pointers" you saw previewed in
Module 03. A common refinement is to **return a status and write results through
output pointers**:

```c
#include <stdio.h>
#include <stdbool.h>

bool divide(int numerator, int denominator, int *quotient, int *remainder) {
    if (denominator == 0) {
        return false;           /* refuse; leave outputs untouched */
    }
    *quotient = numerator / denominator;
    *remainder = numerator % denominator;
    return true;
}

int main(void) {
    int q = 0, r = 0;

    if (divide(17, 5, &q, &r)) {
        printf("17 / 5 = %d remainder %d\n", q, r);
    }
    if (!divide(17, 0, &q, &r)) {
        printf("division by zero refused\n");
    }
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/out_params.c -o out_params
$ ./out_params
17 / 5 = 3 remainder 2
division by zero refused
```

This "return a bool, write the answer through a pointer" shape is everywhere in
real C and in this course's capstone. Get comfortable with it.

---

## 8. Function pointers (brief)

A function has an address too, so you can store it in a pointer and call through it.
This lets you pick behavior at runtime — like passing a function as an argument in
Python.

```c
#include <stdio.h>

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }

int main(void) {
    int (*op)(int, int);      /* "op is a pointer to a function (int,int)->int" */

    op = add;
    printf("add(6, 4) = %d\n", op(6, 4));
    op = sub;
    printf("sub(6, 4) = %d\n", op(6, 4));

    int (*table[3])(int, int) = {add, sub, mul};
    const char *names[3] = {"add", "sub", "mul"};
    for (size_t i = 0; i < 3; i++) {
        printf("%s(6, 4) = %d\n", names[i], table[i](6, 4));
    }
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/func_pointer.c -o func_pointer
$ ./func_pointer
add(6, 4) = 10
sub(6, 4) = 2
add(6, 4) = 10
sub(6, 4) = 2
mul(6, 4) = 24
```

The declaration syntax `int (*op)(int, int)` is notoriously ugly. Read it inside-out:
`op` is a pointer (`*op`), to a function taking `(int, int)`, returning `int`. You'll
mostly meet function pointers as callbacks (e.g. the comparison function you hand to
`qsort`). That's enough for now.

---

## Common mistakes / gotchas

- **Confusing `p` with `*p`.** `p` is the address; `*p` is the value there. Setting
  `p = 5;` is almost always wrong (you overwrite the address with the integer 5);
  you meant `*p = 5;`.
- **Uninitialized pointers.** `int *p;` then `*p = 5;` writes through garbage — a
  *wild pointer* crash. Initialize pointers (`= NULL` or a real address) before use.
- **Returning the address of a local.** The local dies when the function returns, so
  the pointer dangles. GCC warns on the obvious case; heed it. Use the heap (Module 07)
  when you need memory to outlive a function.
- **Forgetting `&` on `scanf`.** `scanf("%d", &n)` needs the *address* of `n`.
  Writing `scanf("%d", n)` passes `n`'s value as if it were an address — a crash.
- **Wrong format specifier.** `%d` for an `int`, `%zu` for `size_t`, `%p` with a
  `(void *)` cast for pointers. Mismatches are undefined behavior, not just ugly.
- **Assuming a pointer is "big."** A pointer is one address (8 bytes on 64-bit).
  `sizeof(ptr)` is the pointer's size, never the size of what it points at.

## Recap

- A pointer is a variable holding a memory address. `&x` gets an address; `*p`
  follows one. They are inverses.
- Writing through a pointer (`*p = ...`) edits the original — no copy. That is how
  functions change the caller's data.
- Variables live in regions (stack / heap / data / bss / text) with different
  lifetimes; a stack variable's address dies when its function returns.
- `p + 1` advances by one element (`sizeof(*p)` bytes). Indexing is just this:
  `a[i] == *(a + i)`.
- `NULL` means "points at nothing"; dereferencing it segfaults. Check before you
  dereference.
- Use `const` for pointers to data you won't modify; use **output parameters** to
  return multiple values. Functions have addresses too — that's a function pointer.

## Exercises

See [`exercises/README.md`](exercises/README.md). Do them before peeking at
[`solutions/`](solutions/). Several are "make it crash, then fix it" — that's the point.

## Further reading

- `man 3 malloc` (for the memory-region picture; we use it properly in Module 07)
- K&R, *The C Programming Language*, Chapter 5 — the classic pointer chapter.
- `man 7 mmap` and `/proc/<pid>/maps` if you want to see your real memory map.
