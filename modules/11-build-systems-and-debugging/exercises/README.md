# Module 11 — Exercises

These exercises use the tools, not just the compiler. Reference solutions are in
[`solutions/`](../solutions).

---

## 1. Write a Makefile

**Goal:** Write a `Makefile` that builds the 3-file project in this folder
([`greeter.h`](greeter.h), [`greeter.c`](greeter.c), [`ex1_main.c`](ex1_main.c)) into a
program called `greet`.

**Constraints:**
- Use `CC`, `CFLAGS` (`-std=c11 -Wall -Wextra -g`), `TARGET`, and `OBJS` variables.
- A pattern rule compiles `%.o` from `%.c` and lists `greeter.h` as a prerequisite.
- Include a `.PHONY` `clean` target. Recipe lines start with a TAB.

**Verify:**
```
$ make
$ ./greet Ada
Hello, Ada!
$ touch greeter.h && make      # only the dependent .o files rebuild, then relink
$ make clean                   # removes greet and the .o files
```

---

## 2. Split a monolith into `.c`/`.h`

**Goal:** [`ex2_mono.c`](ex2_mono.c) crams a helper and `main` into one file. Split it into
`mathutil.h` (declarations, include-guarded), `mathutil.c` (definitions), and `main.c`
(uses them). Build the three together.

**Constraints:**
- The header declares the functions and is safe to include twice (include guard).
- No function is *defined* in the header (one-definition rule).
- `main.c` includes `"mathutil.h"` with quotes.

**Verify:** The split build produces identical output to the original:
```
$ gcc -std=c11 -Wall -Wextra ex2_mono.c -o mono && ./mono 6 7
6 + 7 = 13
6 * 7 = 42
$ gcc -std=c11 -Wall -Wextra main.c mathutil.c -o split && ./split 6 7
6 + 7 = 13
6 * 7 = 42
```

---

## 3. Find the segfault with `gdb`

**Goal:** [`ex3_buggy.c`](ex3_buggy.c) crashes for some inputs. Use `gdb` to find exactly
where and why, then fix it so it never crashes.

**Constraints:**
- Build with `-g`. Reproduce under gdb; use `run`, `backtrace`, and `print` to identify the
  bad value.
- The fix must handle the "not found" case gracefully (no crash, sensible message).

**Verify:**
```
$ ./ex3 hello        # 'l' is present
found 'l'
$ ./ex3 hello z      # (in your fixed version) must NOT crash
'z' not found
```
Write one sentence in a comment naming the line gdb pointed to and the bad value.

---

## 4. Fix the leak with `valgrind`

**Goal:** [`ex4_leaky.c`](ex4_leaky.c) leaks memory. Confirm with valgrind, then fix it so
valgrind reports zero leaks.

**Constraints:**
- Build with `-g`. Run `valgrind --leak-check=full ./ex4` before and after.
- Don't change what the program prints.

**Verify:** After the fix:
```
$ valgrind --leak-check=full ./ex4
...
==...== All heap blocks were freed -- no leaks are possible
==...== ERROR SUMMARY: 0 errors from 0 contexts ...
```

---

## 5. Fix the undefined behavior with a sanitizer

**Goal:** [`ex5_ub.c`](ex5_ub.c) has undefined behavior that UBSan flags at runtime. Build
with `-fsanitize=undefined`, read the report, and fix the root cause.

**Constraints:**
- Build: `gcc -std=c11 -Wall -Wextra -g -fsanitize=undefined ex5_ub.c -o ex5`.
- The fix must compute the intended result without the overflow (not merely silence the
  warning).

**Verify:** The fixed program runs clean under `-fsanitize=undefined,address` with no
runtime error and prints a correct result.
