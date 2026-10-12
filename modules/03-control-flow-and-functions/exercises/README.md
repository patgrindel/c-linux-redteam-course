# Module 03 — Exercises

Standard build:

```bash
gcc -std=c11 -Wall -Wextra FILE.c -o PROG
```

Zero warnings required. Try each before checking [`../solutions/`](../solutions/).

---

## Exercise 1 — FizzBuzz

**Goal.** Write `ex1.c` that prints the numbers 1 through 30, one per line, but prints
`Fizz` for multiples of 3, `Buzz` for multiples of 5, and `FizzBuzz` for multiples of
both.

**Constraints.**
- Use a single `for` loop.
- Decide the three special cases with `if`/`else` (test "both" first).

**Verify.**
```bash
gcc -std=c11 -Wall -Wextra ex1.c -o ex1
./ex1
```
Line 15 must be `FizzBuzz`, line 9 `Fizz`, line 10 `Buzz`, line 1 `1`.

---

## Exercise 2 — Prime checker

**Goal.** Write `ex2.c` with a function `bool is_prime(int n)` and a `main` that prints
every prime from 2 to 29 on one line, space-separated.

**Constraints.**
- `#include <stdbool.h>` and return `bool`.
- `is_prime` takes its argument by value; `main` never sees its internals.
- Numbers less than 2 are not prime.

**Verify.**
```bash
gcc -std=c11 -Wall -Wextra ex2.c -o ex2
./ex2        # expect: 2 3 5 7 11 13 17 19 23 29
```

---

## Exercise 3 — Make swap actually work

**Goal.** [`ex3_starter.c`](ex3_starter.c) has a `swap` that takes its arguments by
value, so it does nothing to the caller's variables. Rewrite it as `ex3.c` so that
after `swap(&x, &y)` the caller's `x` and `y` are genuinely exchanged.

**Constraints.**
- `swap` must take two `int *` and exchange the pointed-to values.
- `main` must pass addresses with `&`.

**Verify.**
```bash
gcc -std=c11 -Wall -Wextra ex3.c -o ex3
./ex3        # must show x and y swapped afterward
```
First build and run the starter to see the by-value version fail — that is the point.

---

## Exercise 4 — Min and max via output pointers

**Goal.** Write `ex4.c` with a function
`void min_max(int a, int b, int c, int *lo, int *hi)` that stores the smallest of the
three into `*lo` and the largest into `*hi`. Call it from `main` and print both.

**Constraints.**
- Return nothing; communicate results only through the two output pointers.
- No arrays needed — just the three parameters.

**Verify.**
```bash
gcc -std=c11 -Wall -Wextra ex4.c -o ex4
./ex4        # for 7, 2, 5 expect: min = 2, max = 7
```

---

## Exercise 5 — Recursive power

**Goal.** Write `ex5.c` with a recursive function `long ipow(int base, int exp)` (for
`exp >= 0`) that computes `base` raised to `exp` without using any library math
function. Print a few results from `main`.

**Constraints.**
- Must be recursive: `ipow(b, e)` defined in terms of `ipow(b, e - 1)`.
- Base case: any number to the power 0 is 1.

**Verify.**
```bash
gcc -std=c11 -Wall -Wextra ex5.c -o ex5
./ex5        # e.g. 2^10 = 1024, 5^3 = 125, 7^0 = 1
```
