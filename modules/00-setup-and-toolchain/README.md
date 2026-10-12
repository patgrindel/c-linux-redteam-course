# Module 00 — Setup & the C Toolchain

*Get a working environment, build your first program, and understand what the
compiler actually does to your source.*

## Where we are

This is the start. You have a Linux shell and `gcc` installed (see
[SETUP.md](../../SETUP.md)); this module turns that into a build you understand.

## Learning objectives

By the end you can:

- Name the four stages that turn a `.c` file into a runnable program.
- Compile and run a C program with `gcc`, and read its exit code with `$?`.
- Explain why `-Wall -Wextra` belongs on *every* build.
- Tell an **error** (the build failed) from a **warning** (it built, but something
  looks wrong) and respond to each.

## From "run the script" to "build the program"

In Python you do one thing: `python hello.py`. The interpreter reads your source
and runs it on the spot. There is no separate artifact — change the file, run it
again, done.

C splits that into two jobs. First you **build**: a tool reads your source and
produces a standalone executable file. Then you **run** that file. The compiler is
gone by the time your program runs; the file does not need C, `gcc`, or your source
to execute. That is the whole trade: more ceremony up front, a fast self-contained
binary at the end.

The tool we drive is `gcc` (the GNU Compiler Collection). One `gcc` command actually
runs a small pipeline of four stages.

### The four stages

```
your.c  ──▶  preprocessor  ──▶  compiler  ──▶  assembler  ──▶  linker  ──▶  ./a.out
             (text edits)       (C → asm)      (asm → .o)      (combine)
```

1. **Preprocessor** — handles the lines starting with `#`. It pastes in the contents
   of `#include` files and substitutes `#define` macros. Its output is still C, just
   with all that text expanded. Pure find-and-replace; it does not understand C.
2. **Compiler** — the real translator. It parses the expanded C, type-checks it, and
   emits **assembly** (human-readable CPU instructions) for your machine.
3. **Assembler** — turns that assembly into an **object file** (`.o`): raw machine
   code, but with the addresses of outside functions (like `printf`) left blank.
4. **Linker** — fills in those blanks by stitching your object file together with the
   C standard library (where `printf` actually lives), producing one executable.

You rarely invoke these separately, but knowing they exist makes error messages
readable: a "undefined reference to `foo`" is the *linker* failing to find a
definition, which is a completely different problem from a *compiler* syntax error.

## Your first build

Here is [`examples/hello.c`](examples/hello.c):

```c
#include <stdio.h>   /* declares printf */

int main(void)
{
    printf("Hello, C!\n");
    return 0;        /* 0 means "success" to the shell */
}
```

Build and run it:

```console
$ gcc -std=c11 -Wall -Wextra examples/hello.c -o hello
$ ./hello
Hello, C!
```

Breaking down the command:

- `gcc` — run the toolchain.
- `-std=c11` — use the C11 language standard (we fix the standard so everyone sees
  the same behavior).
- `-Wall -Wextra` — turn on a broad set of warnings. More on this below.
- `examples/hello.c` — the input source.
- `-o hello` — name the output `hello`. Without `-o`, `gcc` names it `a.out`.

`./hello` runs it. The `./` matters: it says "run the file *hello* in this
directory," not "search my `PATH` for a command named hello."

A silent compile is a *good* thing in C. No news means the build succeeded.

## Exit codes and `$?`

In C, the integer you `return` from `main` becomes the program's **exit code** — the
single number the program hands back to the shell when it ends. The shell stores it
in `$?`. This is how scripts and pipelines tell whether a command worked.

[`examples/exit_code.c`](examples/exit_code.c):

```c
#include <stdio.h>

int main(void)
{
    printf("Doing some work, then failing on purpose.\n");
    return 3;   /* pretend "3" is our code for a particular error */
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/exit_code.c -o exit_code
$ ./exit_code
Doing some work, then failing on purpose.
$ echo $?
3
```

Convention: **0 means success, non-zero means failure.** A program can use different
non-zero codes to mean different errors. Note `$?` only holds values 0–255; it wraps
around for anything larger, so stick to small codes.

Python's analogue is `sys.exit(3)`; a plain `return` from a Python function is not the
same thing. In C, returning from `main` *is* the way you set the exit code.

## Seeing the preprocessor work

[`examples/circle.c`](examples/circle.c) uses a `#define` macro so you can watch the
first stage in isolation:

```c
#include <stdio.h>

#define PI 3.14159265358979   /* a preprocessor macro: pure text substitution */

int main(void)
{
    double radius = 2.0;
    double area = PI * radius * radius;   /* PI becomes the number at compile time */
    printf("area of a circle with radius %.1f is %.4f\n", radius, area);
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/circle.c -o circle
$ ./circle
area of a circle with radius 2.0 is 12.5664
```

Stop `gcc` after the preprocessor with `-E` to see the text it produced:

```console
$ gcc -E examples/circle.c | tail -n 6
int main(void)
{
    double radius = 2.0;
    double area = 3.14159265358979 * radius * radius;
    printf("area of a circle with radius %.1f is %.4f\n", radius, area);
    return 0;
}
```

`PI` is gone — replaced by its text before the compiler ever saw the code. That is all
a macro is.

## Why `-Wall -Wextra` is not optional

C will happily compile code that is almost certainly a mistake. Warnings are the
compiler telling you "this is legal C, but you probably didn't mean it." In a language
with manual memory and no runtime safety net, those hints catch real bugs before they
become crashes or security holes. Treat warnings as errors you haven't hit yet.

`-Wall` turns on the common warnings; `-Wextra` adds more. Despite the name, `-Wall`
is *not* "all warnings" — it is the sensible default set. Use both, always.

Here is code that compiles but is wrong (this is a **fragment to read, not a file to
keep** — it would clutter `examples/` with a warning):

```c
#include <stdio.h>

int main(void)
{
    int x;                    /* never given a value */
    printf("%d\n", x);        /* reading an uninitialized variable */
    return 0;
}
```

Without warnings it builds silently and prints garbage. With `-Wextra` on, `gcc`
speaks up:

```console
$ gcc -std=c11 -Wall -Wextra bad.c -o bad
bad.c: In function 'main':
bad.c:6:5: warning: 'x' is used uninitialized [-Wuninitialized]
    6 |     printf("%d\n", x);
      |     ^~~~~~
```

That one line would have saved you a confusing debugging session. The fix is to
initialize `x`.

## Errors vs. warnings

These are not the same thing:

| | Error | Warning |
|---|---|---|
| Build outcome | **Fails** — no executable produced | **Succeeds** — executable is produced |
| Meaning | The code is not valid C | The code is valid but suspicious |
| Your job | You *must* fix it to build | You should almost always fix it anyway |

An error looks like this — forget the semicolon after `return 0`:

```console
$ gcc -std=c11 -Wall -Wextra hello.c -o hello
hello.c: In function 'main':
hello.c:7:13: error: expected ';' before '}' token
    7 |     return 0
      |             ^
      |             ;
```

Read errors **top to bottom** and fix the **first** one first. One real mistake often
cascades into several later messages that vanish once you fix the root cause. The
message gives you the file, the line (`7`), the column, and a caret `^` pointing at the
spot — `gcc` is even suggesting the missing `;`.

If you want the compiler to *refuse* to build on warnings too (a great habit), add
`-Werror`, which promotes every warning to an error.

## Common mistakes / gotchas

- **Forgetting `./`.** Typing `hello` searches your `PATH` and fails with "command not
  found." You must say `./hello`.
- **Editing the source but running the old binary.** C does not re-build for you.
  Every source change needs a fresh `gcc` before `./hello`. If a change seems to have
  no effect, you probably forgot to rebuild.
- **Missing the newline.** `printf("Hello")` with no `\n` leaves the cursor mid-line;
  your shell prompt appears glued to the output. C does not add newlines for you the
  way Python's `print` does.
- **Ignoring warnings.** "It compiled" is not "it's correct." A warning you skip today
  is a segfault next week.
- **Confusing compiler and linker failures.** "undefined reference to ..." is the
  linker; a syntax complaint is the compiler. They need different fixes.

## Recap

- `gcc` runs a pipeline: **preprocessor → compiler → assembler → linker**, producing a
  standalone executable you run separately.
- `gcc -std=c11 -Wall -Wextra file.c -o name` is your standard build; run it with
  `./name`.
- `main`'s return value is the process **exit code**; read it with `echo $?`. 0 is
  success.
- **Warnings** mean "builds but suspicious"; **errors** mean "did not build." Fix the
  first error first, and fix warnings too.

## Exercises

See [`exercises/README.md`](exercises/README.md).

## Further reading

- `man gcc` — the compiler's options (huge; skim the "Overall Options" and "Warning
  Options" sections).
- `man 3 printf` — the function you just used, and every format specifier it accepts.
- [GCC Warning Options](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html) — what
  `-Wall` and `-Wextra` actually turn on.
