# Module 11 — Real Projects: Make, Headers & Debugging

Turning a pile of `.c` files into an organized, incrementally-built project — and the tools
(`gdb`, `valgrind`, sanitizers) that find the bugs the compiler can't.

## Where we are

Every module so far was one `.c` file you compiled by hand. The capstone (Module 12) is a
multi-file program. This module teaches the three things that make that manageable:
splitting code across files with headers, building it with `make`, and debugging it when it
misbehaves.

## Learning objectives

By the end you can:

- Split a program into `.c`/`.h` files, use **include guards**, and respect the
  **one-definition rule**.
- Write a `Makefile` with variables, pattern rules, and a `clean` target, and benefit from
  **incremental builds**.
- Use `gdb` to set breakpoints, step, inspect variables, and read a backtrace after a crash.
- Use `valgrind` to find leaks and invalid memory access, and the compiler's
  **sanitizers** (`-fsanitize=address,undefined`) for fast runtime checking.
- Lay out a small project cleanly.

---

## Splitting into `.c` and `.h`

A **header** (`.h`) holds *declarations* — the promises: function prototypes, `struct`
definitions, `typedef`s, `#define`s, `extern` globals. A **source** file (`.c`) holds the
*definitions* — the actual code. Other `.c` files `#include` the header to learn the
promises; the linker later connects each call to the one real definition.

The rule that keeps this sane is the **one-definition rule (ODR)**: a function or global
variable may be *declared* many times but *defined* exactly once across the whole program.
Put declarations in headers; put definitions in exactly one `.c`.

### Include guards

If `a.h` includes `b.h`, and your `.c` includes both, `b.h`'s contents would be pasted
twice — a re-definition error for the types inside. An **include guard** makes a header
safe to include any number of times:

```c
#ifndef STATS_H        /* if this symbol isn't defined yet... */
#define STATS_H        /* ...define it, and include the body once */

/* declarations here */

#endif /* STATS_H */
```

The second time the preprocessor sees the file, `STATS_H` is already defined, so everything
between `#ifndef` and `#endif` is skipped. (You'll also see `#pragma once`; it's widely
supported but non-standard. Guards are universal — this course uses them.)

### The worked 3-file project

[`examples/stats_project/`](examples/stats_project) computes min/max/mean of the numbers you
pass on the command line. Three files plus a `Makefile`.

`stats.h` — the interface:

```c
#ifndef STATS_H
#define STATS_H

#include <stddef.h>   /* size_t */

/* Summary statistics for a set of doubles. */
typedef struct {
    double min;
    double max;
    double mean;
    size_t count;
} stats_t;

/* Compute stats over values[0..n). Requires n > 0. */
stats_t stats_compute(const double *values, size_t n);

#endif /* STATS_H */
```

`stats.c` — the implementation (the single definition of `stats_compute`):

```c
#include "stats.h"

stats_t stats_compute(const double *values, size_t n) {
    stats_t s = { values[0], values[0], 0.0, n };
    double sum = 0.0;
    for (size_t i = 0; i < n; i++) {
        if (values[i] < s.min) {
            s.min = values[i];
        }
        if (values[i] > s.max) {
            s.max = values[i];
        }
        sum += values[i];
    }
    s.mean = sum / (double) n;
    return s;
}
```

`main.c` — the program that uses it:

```c
#include <stdio.h>
#include <stdlib.h>
#include "stats.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <num> [num...]\n", argv[0]);
        return 1;
    }

    size_t n = (size_t) (argc - 1);
    double *values = malloc(n * sizeof *values);
    if (values == NULL) {
        perror("malloc");
        return 1;
    }

    for (size_t i = 0; i < n; i++) {
        values[i] = strtod(argv[i + 1], NULL);
    }

    stats_t s = stats_compute(values, n);
    printf("count=%zu min=%.3f max=%.3f mean=%.3f\n", s.count, s.min, s.max, s.mean);

    free(values);
    return 0;
}
```

Note `#include "stats.h"` with **quotes** (your own headers, searched locally) versus
`#include <stdio.h>` with **angle brackets** (system headers).

You *could* build it by naming both sources:

```
$ gcc -std=c11 -Wall -Wextra stats.c main.c -o stats
$ ./stats 3 1 4 1 5 9 2 6
count=8 min=1.000 max=9.000 mean=3.875
```

But as projects grow, recompiling everything every time is wasteful. That's what `make` is
for.

---

## Writing a `Makefile`

`make` builds targets from prerequisites using rules, and — crucially — only rebuilds what
changed, by comparing file timestamps. Here is the fully annotated
[`examples/stats_project/Makefile`](examples/stats_project/Makefile):

```make
# ---- Variables -------------------------------------------------------------
CC      := gcc                       # the compiler
CFLAGS  := -std=c11 -Wall -Wextra -g # flags for every compile; -g adds debug info
TARGET  := stats                     # the program we want to produce
OBJS    := main.o stats.o            # one .o per .c

# ---- Default target --------------------------------------------------------
# The FIRST target is what `make` with no arguments builds. ".PHONY" targets
# aren't files; declare them so make never skips them because a file matches.
.PHONY: all clean
all: $(TARGET)

# ---- Link step -------------------------------------------------------------
# $(TARGET) depends on all object files; if any is newer, relink.
# $@ = the target name, $^ = all prerequisites.
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# ---- Compile step (pattern rule) -------------------------------------------
# "%.o from the matching %.c" plus the headers. $< = the first prerequisite.
# Listing stats.h means: if the header changes, dependent .o files rebuild.
%.o: %.c stats.h
	$(CC) $(CFLAGS) -c -o $@ $<

# ---- Housekeeping ----------------------------------------------------------
clean:
	rm -f $(TARGET) $(OBJS)
```

> **Tabs, not spaces.** Each recipe line (the indented command under a rule) must begin
> with a real **TAB** character. Spaces give the classic `*** missing separator` error.

Build and watch incremental rebuilds:

```
$ cd examples/stats_project
$ make
gcc -std=c11 -Wall -Wextra -g -c -o main.o main.c
gcc -std=c11 -Wall -Wextra -g -c -o stats.o stats.c
gcc -std=c11 -Wall -Wextra -g -o stats main.o stats.o

$ make                 # nothing changed -> nothing to do
make: Nothing to be done for 'all'.

$ touch stats.c        # pretend we edited stats.c
$ make                 # only stats.o recompiles, then relink
gcc -std=c11 -Wall -Wextra -g -c -o stats.o stats.c
gcc -std=c11 -Wall -Wextra -g -o stats main.o stats.o

$ make clean
rm -f stats main.o stats.o
```

That's the payoff: edit one file in a big project and `make` rebuilds just that object and
relinks, instead of recompiling everything. `CFLAGS` in one place means you flip on a
sanitizer or change the standard for the whole project at once —
`make CFLAGS="-std=c11 -Wall -Wextra -fsanitize=address -g"`.

---

## Debugging with `gdb`

`gdb` runs your program under a debugger so you can pause it, look around, and see exactly
where and why it failed. **Always compile with `-g`** (debug info) and ideally `-O0` (no
optimization) so line numbers and variables map to your source. Our `CFLAGS` already has
`-g`.

### A backtrace on a segfault

[`examples/segfault_demo.c`](examples/segfault_demo.c) crashes on purpose — it counts
characters in a string that is `NULL`:

```c
#include <stddef.h>
#include <stdio.h>

static size_t count_char(const char *s, char c) {
    size_t n = 0;
    while (*s) {                 /* dereferences s; crashes if s is NULL */
        if (*s == c) {
            n++;
        }
        s++;
    }
    return n;
}

int main(void) {
    const char *name = NULL;     /* the bug: should point at a real string */
    printf("count = %zu\n", count_char(name, 'a'));
    return 0;
}
```

```
$ gcc -std=c11 -Wall -Wextra -g examples/segfault_demo.c -o segfault_demo
$ ./segfault_demo
Segmentation fault (core dumped)
```

"Segmentation fault" with no detail is useless. Run it under `gdb`:

```
$ gdb ./segfault_demo
(gdb) run
Starting program: /home/you/segfault_demo

Program received signal SIGSEGV, Segmentation fault.
0x0000555555555159 in count_char (s=0x0, c=97 'a') at segfault_demo.c:6
6	    while (*s) {
(gdb) backtrace
#0  count_char (s=0x0, c=97 'a') at segfault_demo.c:6
#1  0x00005555555551b4 in main () at segfault_demo.c:17
(gdb) print s
$1 = 0x0
(gdb) frame 1
#1  0x00005555555551b4 in main () at segfault_demo.c:17
17	    printf("count = %zu\n", count_char(name, 'a'));
(gdb) print name
$2 = 0x0
(gdb) quit
```

Read it top-down: the crash is at `segfault_demo.c:6`, in `count_char`, and gdb even shows
`s=0x0` — a NULL pointer. `backtrace` (or `bt`) shows the call chain: `main` at line 17
called `count_char`. `frame 1` moves up to `main`, where `print name` confirms `name` is
`0x0`. You've found the bug in under a minute without adding a single `printf`.

### Breakpoints and stepping

The everyday gdb loop — pause, inspect, advance:

```
$ gdb ./stats
(gdb) break stats_compute        # pause when this function is entered
Breakpoint 1 at 0x...: file stats.c, line 5.
(gdb) run 3 1 4                  # program args go after `run`
Breakpoint 1, stats_compute (values=0x..., n=3) at stats.c:5
5	    stats_t s = { values[0], values[0], 0.0, n };
(gdb) print n                    # inspect a variable
$1 = 3
(gdb) next                       # run this line, stay in this function
(gdb) step                       # run this line, stepping INTO any call
(gdb) print values[1]            # inspect an array element
$2 = 1
(gdb) continue                   # run to the next breakpoint (or the end)
count=3 min=1.000 max=4.000 mean=2.667
[Inferior 1 (process ...) exited normally]
(gdb) quit
```

Essentials: `break <func>`/`break <file>:<line>`, `run [args]`, `next` (over calls),
`step` (into calls), `print <expr>`, `backtrace`, `continue`, `finish` (run until the
current function returns), `info locals`. On a crash you don't even need a breakpoint — just
`run`, then `backtrace`.

---

## `valgrind`: leaks and invalid memory

`valgrind` runs your program on a synthetic CPU and watches every memory access. It finds
leaks, use-after-free, double-free, and reads/writes of uninitialized or out-of-bounds
memory — the bugs that otherwise "work on my machine" and corrupt data later. Compile with
`-g` so it can name lines.

[`examples/leak_demo.c`](examples/leak_demo.c) leaks on purpose:

```c
#include <stdlib.h>
#include <string.h>

int main(void) {
    char *buf = malloc(64);       /* allocated... */
    strcpy(buf, "hello");
    /* ...and never freed: a leak */
    return 0;
}
```

```
$ gcc -std=c11 -Wall -Wextra -g examples/leak_demo.c -o leak_demo
$ valgrind --leak-check=full ./leak_demo
==12345== HEAP SUMMARY:
==12345==     in use at exit: 64 bytes in 1 blocks
==12345==   total heap usage: 1 allocs, 0 frees, 64 bytes allocated
==12345==
==12345== 64 bytes in 1 blocks are definitely lost in loss record 1 of 1
==12345==    at 0x...: malloc (vg_replace_malloc.c:...)
==12345==    by 0x...: main (leak_demo.c:5)
==12345==
==12345== LEAK SUMMARY:
==12345==    definitely lost: 64 bytes in 1 blocks
==12345== ERROR SUMMARY: 1 errors from 1 contexts ...
```

"definitely lost: 64 bytes ... at main (leak_demo.c:5)" points straight at the `malloc`
whose memory was never freed. Add `free(buf);` and valgrind prints "All heap blocks were
freed -- no leaks are possible". The goal on a clean program is **"ERROR SUMMARY: 0
errors"**.

---

## Sanitizers: fast checks built into the compiler

Sanitizers are instrumentation the *compiler* adds to your binary. They're much faster than
valgrind and catch slightly different things, so they're ideal during development. Turn them
on with `-fsanitize=...` (and keep `-g` for line numbers):

- **AddressSanitizer** (`-fsanitize=address`, "ASan") — out-of-bounds, use-after-free,
  some leaks.
- **UndefinedBehaviorSanitizer** (`-fsanitize=undefined`, "UBSan") — signed overflow,
  invalid shifts, misaligned pointers, and more.

You usually combine them: `-fsanitize=address,undefined`.

[`examples/ub_demo.c`](examples/ub_demo.c) writes one past the end of a stack array:

```c
#include <stdio.h>

int main(void) {
    int a[4] = {0};
    int sum = 0;
    for (int i = 0; i <= 4; i++) {   /* BUG: i == 4 is out of bounds */
        a[i] = i;
        sum += a[i];
    }
    printf("sum = %d\n", sum);
    return 0;
}
```

Without sanitizers it may *appear* to work (that's what makes these bugs dangerous). With
ASan it stops at the exact line:

```
$ gcc -std=c11 -Wall -Wextra -g -fsanitize=address,undefined examples/ub_demo.c -o ub_demo
$ ./ub_demo
=================================================================
==12345==ERROR: AddressSanitizer: stack-buffer-overflow on address 0x...
WRITE of size 4 at 0x... thread T0
    #0 0x... in main ub_demo.c:7
...
  'a' (line 4) <== Memory access at offset ... overflows this variable
==12345==ABORTING
```

`ub_demo.c:7`, a write that "overflows this variable 'a'". Fix the loop bound to `i < 4`
and it runs clean. A good habit: keep a sanitized build target in your `Makefile` and run
your tests under it.

---

## Project hygiene

A small, sane layout (what the capstone uses):

```
myproject/
├── Makefile
├── README.md
├── src/          # .c files
├── include/      # .h files   (add -Iinclude to CFLAGS)
└── tests/        # test programs or scripts
```

Habits that pay off:

- **Warnings are errors in spirit.** Build with `-Wall -Wextra`; consider `-Werror` once
  clean. A warning is the compiler telling you about a real bug most of the time.
- **One module, one responsibility.** `stats.c` does stats; `main.c` does argument handling
  and output. Headers expose only what callers need.
- **Check every call** (Modules 08–10) and free what you allocate (Module 07) — then prove
  it with valgrind/ASan.
- **Keep the build reproducible.** All flags live in the `Makefile`; `make clean && make`
  should always work from scratch.
- **A short `README`** per project: what it does, how to build, how to run.

---

## Common mistakes / gotchas

- **Spaces instead of a TAB** before a Makefile recipe line → `*** missing separator`.
- **Defining a function or global in a header** that's included by multiple `.c` files →
  duplicate-definition link errors. Headers declare; `.c` files define. (Exceptions:
  `static inline`, `typedef`, macros.)
- **No include guard** → "redefinition of struct ..." when a header is included twice.
- **Debugging an optimized binary.** Without `-g` (and ideally `-O0`) gdb shows `??` and
  wrong lines. Build a debug configuration.
- **Not listing header prerequisites in the Makefile.** Edit a header, and dependent `.o`
  files don't rebuild — you debug stale code. List the `.h` in the rule (or generate
  dependencies with `-MMD`).
- **Treating "valgrind clean" and "ASan clean" as the same.** They overlap but each catches
  things the other misses; run both when it matters.
- **Shipping with `-fsanitize=address`.** Great for dev and tests; it slows the program and
  changes its memory layout. Build release binaries without it.

## Recap

- Headers hold declarations; `.c` files hold definitions; each function/global is defined
  **once** (ODR). Guard every header with `#ifndef`/`#define`/`#endif`.
- `make` rebuilds only what changed. Put the compiler, `CFLAGS`, and object list in
  variables; declare `.PHONY` targets; recipes start with a TAB.
- `gdb` (`-g`): `run`, `backtrace` on a crash, `break`/`next`/`step`/`print` to explore.
- `valgrind` finds leaks and invalid memory; sanitizers (`-fsanitize=address,undefined`)
  catch overlapping bugs faster. Aim for zero errors from both.
- Keep a clean layout, warnings on, errors checked, memory freed.

## Exercises

See [`exercises/README.md`](exercises/README.md).

## Further reading

- `man` pages: `make(1)`, `gcc(1)`, `gdb(1)`, `valgrind(1)`.
- The GNU Make manual (variables, pattern rules, automatic variables `$@ $^ $<`).
- *Debugging with GDB* (the official manual); the ASan/UBSan pages in the GCC/Clang docs.
