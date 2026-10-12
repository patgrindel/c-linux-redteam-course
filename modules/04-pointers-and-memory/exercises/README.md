# Module 04 — Exercises

Build every solution with:

```console
gcc -std=c11 -Wall -Wextra ex1.c -o ex1
```

No warnings allowed. Try each before looking at `solutions/`.

---

## Exercise 1 — Increment through a pointer

**Goal.** Write `void increment(int *p)` that adds 1 to the int `p` points at. In
`main`, make an `int x = 10;`, print it, call `increment(&x)`, and print it again.

**Constraints.**
- `increment` must not return anything; it changes `x` *through the pointer*.
- Print the address of `x` once with `%p` (cast to `(void *)`).

**Verify.** Output shows `x` going from 10 to 11. Starter: `ex1_starter.c`.

---

## Exercise 2 — Three-way rotate (output parameters)

**Goal.** Write `void rotate3(int *a, int *b, int *c)` that moves the values left:
`a` gets the old `b`, `b` gets the old `c`, `c` gets the old `a`.

**Constraints.**
- Use only pointers; no global variables.
- In `main`, start with `1, 2, 3` and print before and after.

**Verify.** `1 2 3` becomes `2 3 1`.

---

## Exercise 3 — Sum an array with pointers only

**Goal.** Write `int sum(const int *first, const int *last)` that returns the sum of
the ints in the half-open range `[first, last)` — i.e. walk a pointer from `first`
up to (but not including) `last`. No indexing with `[]` anywhere in `sum`.

**Constraints.**
- `sum` must use `const`.
- Call it as `sum(nums, nums + 5)` for a 5-element array.

**Verify.** For `{10, 20, 30, 40, 50}` the result is `150`.

---

## Exercise 4 — Min and max via output parameters

**Goal.** Write `bool min_max(const int *a, size_t n, int *out_min, int *out_max)`.
It writes the smallest and largest element through the output pointers and returns
`true`. If `n == 0` it writes nothing and returns `false`.

**Constraints.**
- Use `#include <stdbool.h>`.
- Loop index must be `size_t` to match `n` (avoids a signed/unsigned warning).

**Verify.** For `{4, -2, 9, 0, 9}`: min `-2`, max `9`, returns true. For `n == 0`:
returns false and your `main` prints "empty".

---

## Exercise 5 — Function-pointer calculator

**Goal.** Reuse the `add`/`sub`/`mul` idea. Write `int apply(int (*op)(int, int),
int x, int y)` that calls `op(x, y)` and returns the result. In `main`, call
`apply` with each of `add`, `sub`, `mul`.

**Constraints.**
- `apply` takes the function pointer as its first parameter.
- No `if`/`switch` inside `apply` — the whole point is that the behavior is the
  argument.

**Verify.** `apply(add,6,4)=10`, `apply(sub,6,4)=2`, `apply(mul,6,4)=24`.
