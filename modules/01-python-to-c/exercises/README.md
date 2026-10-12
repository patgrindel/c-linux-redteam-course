# Module 01 — Exercises

Standard build:

```bash
gcc -std=c11 -Wall -Wextra FILE.c -o PROG
```

(Exercise 3 builds *two* files at once — see its instructions.)

Try each before looking at [`../solutions/`](../solutions/).

---

## Exercise 1 — Success or failure by argument count

**Goal.** Write `ex1.c` that checks a variable `int record_count` you set in the
source. If it is greater than 0, print `found N records` and return the success code;
otherwise print `no records` and return the failure code.

**Constraints.**
- Use `EXIT_SUCCESS` / `EXIT_FAILURE` from `<stdlib.h>`.
- Print with `printf` and a trailing `\n`.

**Verify.**
```bash
gcc -std=c11 -Wall -Wextra ex1.c -o ex1
./ex1 ; echo $?
```
Set `record_count` to a positive number, confirm `$?` is 0; set it to 0, confirm `$?`
is non-zero.

---

## Exercise 2 — Prototype first

**Goal.** Write `ex2.c` containing a function `int cube(int n)` that returns `n*n*n`.
Call it from `main` for the values 2 and 3. The catch: **define `cube` below `main`**,
so you must write a prototype above `main` for the program to compile.

**Constraints.**
- The definition of `cube` must appear *after* `main` in the file.
- The build must be warning-free (a missing prototype is a warning/error).

**Verify.**
```bash
gcc -std=c11 -Wall -Wextra ex2.c -o ex2
./ex2        # expect: 2 cubed is 8  /  3 cubed is 27
```

---

## Exercise 3 — Split a program into header + implementation

**Goal.** You are given a single-file program, [`ex3_combined.c`](ex3_combined.c),
that defines and uses `int add(int, int)`. Split it into three files:
- `mathutil.h` — the declaration of `add`, with an include guard.
- `mathutil.c` — the definition of `add`.
- `ex3.c` — `main`, which includes `mathutil.h` and calls `add`.

**Constraints.**
- `ex3.c` must not contain the body of `add`.
- Use an include guard in the header.

**Verify.**
```bash
gcc -std=c11 -Wall -Wextra ex3.c mathutil.c -o ex3
./ex3
```
Then prove you understand the linker: build with only `gcc ... ex3.c -o ex3` (omit
`mathutil.c`) and read the "undefined reference to `add`" error. That is the linker,
not the compiler.

---

## Exercise 4 — Translate Python to C

**Goal.** Write `ex4.c` that reproduces this Python program's output exactly:

```python
total = 0
for i in range(1, 11):
    total += i
print(f"sum of 1..10 is {total}")
```

**Constraints.**
- Declare `total` and `i` with explicit types.
- Match the output line character-for-character (including the final newline).

**Verify.**
```bash
gcc -std=c11 -Wall -Wextra ex4.c -o ex4
./ex4        # expect: sum of 1..10 is 55
```
