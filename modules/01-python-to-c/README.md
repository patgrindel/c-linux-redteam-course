# Module 01 — From Python to C: The Compilation Model

*The mental shift from an interpreted, dynamic language to a compiled, statically
typed one — and what that buys you and costs you.*

## Where we are

In Module 00 you drove `gcc` through its four stages and ran a binary. Now we zoom
out: *why* C works this way, and how its model differs from the Python you already
know.

## Learning objectives

By the end you can:

- Explain what a compiled binary *is* and how it differs from running a script.
- Describe `main` and use its `return` value as the program's exit code.
- Use `#include` and explain the role of headers.
- Distinguish a **declaration** from a **definition** and say why C needs both.
- Explain, concretely, what "static typing, manual memory, no garbage collector"
  means for how you write code.

## Compiled vs. interpreted

When you run `python app.py`, the `python` program is still in memory the whole time,
reading your source and executing it statement by statement. Your `.py` file *is* the
program; there is no separate build step and no standalone artifact. Ship the code,
ship the interpreter.

C is **ahead-of-time compiled**. `gcc` translates your entire source into machine code
*before* the program runs, producing an executable file. When you run that file, the
CPU executes its instructions directly — `gcc` is long gone and plays no part.

| | Python (interpreted) | C (compiled) |
|---|---|---|
| When your code is translated | While it runs, each time | Once, by `gcc`, before running |
| What you ship | Source + an interpreter | A self-contained binary |
| When type errors surface | At runtime, if that line runs | At **compile** time, before running |
| Speed | Slower (translation overhead) | Fast (native machine code) |
| Safety net | Big runtime (bounds checks, GC) | Almost none — you are the safety net |

The trade is stark: C makes you do more work up front and removes the guardrails, in
exchange for speed and direct control of the machine. That control is exactly why C is
the language of operating systems, and why it is worth learning for low-level security
work.

## What the binary *is*

The file `gcc` produces is not text and not "bytecode for a C virtual machine." It is
a block of real instructions for your CPU plus some bookkeeping, packaged in a format
Linux understands (ELF, the Executable and Linkable Format). You can prove there is no
interpreter involved:

```console
$ gcc -std=c11 -Wall -Wextra examples/exit_status.c -o exit_status
$ file exit_status
exit_status: ELF 64-bit LSB pie executable, x86-64, ...
$ ./exit_status
processed 7 items
```

`file` reports a native executable. Nothing named "c" needs to be installed to run it;
you could copy this file to another matching Linux box with no compiler and it would
run. That is what "compiled to a binary" means.

## `main` and the exit code

Every C program starts at a function named `main`. You do not call it — the system
does, once, when the program launches. Whatever `int` it returns becomes the exit code
the shell reads in `$?` (you saw this in Module 00).

[`examples/exit_status.c`](examples/exit_status.c) uses the self-documenting names from
`<stdlib.h>`:

```c
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int items_processed = 7;

    if (items_processed == 0) {
        printf("nothing to do\n");
        return EXIT_FAILURE;   /* conventionally non-zero */
    }

    printf("processed %d items\n", items_processed);
    return EXIT_SUCCESS;       /* conventionally 0 */
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/exit_status.c -o exit_status
$ ./exit_status ; echo $?
processed 7 items
0
```

`EXIT_SUCCESS` is `0` and `EXIT_FAILURE` is some non-zero value; using the names makes
intent obvious. The signature `int main(void)` says "takes no arguments, returns an
int." (`void` in the parentheses means "no parameters" — we meet `main(int, char**)`
for command-line arguments in Module 05.)

## `#include` and headers

The first lines of almost every C file look like `#include <stdio.h>`. In Module 00 you
learned the preprocessor literally pastes the named file's text in at that point. So
what is in `stdio.h`, and why do we need it?

C enforces a rule: **you may not use a function the compiler has not already seen
declared.** `printf` lives in the C standard library, whose machine code is linked in
at the end — but the *compiler* still needs to know `printf`'s shape (its return type
and parameters) to check your call. That knowledge lives in the **header** `stdio.h`.

So a header is an interface document: it *declares* what exists without *defining* how
it works. Angle brackets `<stdio.h>` search the system include paths; double quotes
`"greet.h"` search your project first (for your own headers).

Rough Python analogy: `#include` feels like `import`. **Where it breaks:** Python's
`import` happens at runtime, loads a real module object, and runs that module's code.
C's `#include` is a compile-time *text paste* of declarations only — no code runs, and
the actual implementation is bolted on later by the linker. Two separate mechanisms
(include for the compiler, linking for the implementation) doing what Python's import
does in one.

## Declarations vs. definitions

This distinction trips up every newcomer, so meet it head-on.

- A **declaration** introduces a name and its type: "a function `square` exists; it
  takes an `int` and returns an `int`." It makes a promise.
- A **definition** provides the actual thing: the function body, or the storage for a
  variable. It keeps the promise.

The compiler reads a file top to bottom and must know a function's shape *before* the
first call. [`examples/decl_vs_def.c`](examples/decl_vs_def.c) declares `square` up top
(a **prototype**) so `main` can call it, and defines it below:

```c
#include <stdio.h>

int square(int n);   /* declaration (prototype): "square exists, trust me" */

int main(void)
{
    int result = square(9);
    printf("9 squared is %d\n", result);
    return 0;
}

int square(int n)    /* definition: the actual code */
{
    return n * n;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/decl_vs_def.c -o decl_vs_def
$ ./decl_vs_def
9 squared is 81
```

A name may be **declared** many times (every file that calls `square` declares it) but
**defined** exactly once across the whole program. That "one definition" rule is why
headers hold declarations and `.c` files hold definitions.

### Headers in practice: a two-file program

This is exactly how real C projects split up. The *interface* goes in a `.h`; the
*implementation* in a `.c`; code that uses it includes the header.

[`examples/greet.h`](examples/greet.h) — the interface:

```c
#ifndef GREET_H          /* include guard: prevents double-inclusion */
#define GREET_H

void greet(const char *name);

#endif /* GREET_H */
```

[`examples/greet.c`](examples/greet.c) — the implementation:

```c
#include <stdio.h>
#include "greet.h"

void greet(const char *name)
{
    printf("Hello, %s! Welcome to C.\n", name);
}
```

[`examples/greet_main.c`](examples/greet_main.c) — the user:

```c
#include "greet.h"

int main(void)
{
    greet("world");
    greet("red team");
    return 0;
}
```

Build it by handing both `.c` files to `gcc`, which compiles each and links them:

```console
$ gcc -std=c11 -Wall -Wextra examples/greet_main.c examples/greet.c -o greet
$ ./greet
Hello, world! Welcome to C.
Hello, red team! Welcome to C.
```

`greet_main.c` never sees the body of `greet` — only its declaration from the header.
The linker connects the call to the definition in `greet.c`. If you forget to pass
`greet.c`, the compile succeeds but the **linker** fails with "undefined reference to
`greet`" — the promise was made but never kept. (The `#ifndef/#define/#endif` include
guard ensures that if a header gets included twice, its contents are only pasted once;
we rely on it heavily from Module 11 on.)

## Static typing, manual memory, no GC

Three phrases describe most of what makes C feel different. Here is what each means for
*you*, day to day.

### Static typing

Every variable has exactly one type, fixed when you declare it and checked at compile
time. [`examples/static_typing.c`](examples/static_typing.c):

```c
#include <stdio.h>

int main(void)
{
    int   count   = 42;
    double price  = 9.99;
    char  initial = 'C';

    printf("count   = %d    (%zu bytes)\n", count, sizeof count);
    printf("price   = %.2f  (%zu bytes)\n", price, sizeof price);
    printf("initial = %c     (%zu bytes)\n", initial, sizeof initial);
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/static_typing.c -o static_typing
$ ./static_typing
count   = 42    (4 bytes)
price   = 9.99  (8 bytes)
initial = C     (1 bytes)
```

In Python, `x = 42` then `x = "hello"` is fine — the name just rebinds. In C, `count`
is an `int` forever; `count = "hello";` is a compile error, caught before the program
ever runs. The upside: a whole class of bugs is impossible, and the compiler knows
every variable's exact size (hence `sizeof` works at compile time). The cost: you must
pick types deliberately — the subject of Module 02. (`sizeof` yields a `size_t`, which
is why we print it with `%zu`.)

### Manual memory and no garbage collector

Python tracks references and frees objects automatically; you never think about it. C
has **no garbage collector.** Memory you request from the system with `malloc` stays
yours until you explicitly `free` it. [`examples/manual_memory.c`](examples/manual_memory.c)
shows the shape of it (full treatment in Modules 04 and 07):

```c
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *numbers = malloc(5 * sizeof *numbers);
    if (numbers == NULL) {
        fprintf(stderr, "out of memory\n");
        return 1;
    }

    for (int i = 0; i < 5; i++) {
        numbers[i] = i * i;
    }
    for (int i = 0; i < 5; i++) {
        printf("numbers[%d] = %d\n", i, numbers[i]);
    }

    free(numbers);   /* without this, the block leaks */
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/manual_memory.c -o manual_memory
$ ./manual_memory
numbers[0] = 0
numbers[1] = 1
numbers[2] = 4
numbers[3] = 9
numbers[4] = 16
```

For every `malloc` there must be exactly one `free`. Too few: a **leak**. Freeing twice
or using memory after freeing it: **undefined behavior** — crashes or silent
corruption. This discipline is the heart of C, and the source of a huge fraction of
real-world security vulnerabilities. We build the skill carefully over the next
several modules.

## A side-by-side tour

The same tiny program, printing squares of 0–4:

```python
# squares.py
def square(n):
    return n * n

for i in range(5):
    print(f"{i} -> {square(i)}")
```

```c
/* squares.c */
#include <stdio.h>

int square(int n) {
    return n * n;
}

int main(void) {
    for (int i = 0; i < 5; i++) {
        printf("%d -> %d\n", i, square(i));
    }
    return 0;
}
```

Point-by-point:

| Idea | Python | C |
|---|---|---|
| Entry point | top-level code runs | execution starts at `main` |
| Printing | `print(...)` adds a newline | `printf(...)`; you add `\n` yourself |
| String formatting | f-strings | format specifiers (`%d`, `%f`, ...) |
| Variable types | inferred, dynamic | declared, static (`int i`) |
| Blocks | indentation | braces `{ }`; indentation is just style |
| Loop over a range | `for i in range(5)` | `for (int i = 0; i < 5; i++)` |
| Statement end | newline | semicolon `;` |
| Running it | `python squares.py` | `gcc ... && ./squares` |

The logic is identical; C just forces you to state what Python infers. Once the syntax
stops surprising you, the real learning is the model underneath — types, memory,
and the machine — which is the rest of this course.

## Common mistakes / gotchas

- **Expecting a type to change.** `int x;` is an int for life. Reassigning a different
  type is an error, not a rebind.
- **Forgetting the `\n`.** `printf` prints exactly what you give it. No automatic
  newline like `print`.
- **Calling a function the compiler hasn't seen.** Put a prototype above `main`, or
  `#include` the header that declares it. Otherwise you get an implicit-declaration
  error.
- **Compiling one file when you have two.** `gcc main.c` alone fails to link if `main.c`
  calls code defined in `other.c`. List every `.c`: `gcc main.c other.c -o prog`.
- **Confusing `#include` with `import`.** Including a header gives you *declarations* so
  the compiler is happy; it does not supply the implementation. That is the linker's
  job, from the matching `.c` or a library.
- **Expecting garbage collection.** Nothing frees your `malloc` for you.

## Recap

- C is compiled ahead of time into a **self-contained native binary**; Python is
  interpreted, carrying its runtime along.
- Execution begins at `main`; its returned `int` is the process **exit code**.
- `#include` pastes in a **header** of declarations so the compiler can check your
  calls; the **linker** later supplies the implementations.
- A **declaration** promises a name's type; a **definition** provides it. Declare often,
  define once.
- **Static typing** is checked before you run; **manual memory** with **no GC** means
  every `malloc` is your responsibility to `free`.

## Exercises

See [`exercises/README.md`](exercises/README.md).

## Further reading

- `man 3 exit` and `man 3 printf` — the exit/print functions and the `EXIT_*` macros.
- `man gcc` — revisit "undefined reference" under the linker's behavior.
- *The C Programming Language* (Kernighan & Ritchie), Chapter 1 — the classic guided
  tour; terse but worth it.
