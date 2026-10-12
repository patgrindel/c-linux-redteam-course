# Module 07 — Exercises

Build with `gcc -std=c11 -Wall -Wextra -g exN.c -o exN`, then run
`valgrind ./exN`. A correct solution produces **0 errors** and **no leaks**. Try
before peeking at `solutions/`.

---

## Exercise 1 — Read an unknown number of ints, print them reversed

**Goal.** Read integers from standard input until EOF into a dynamically grown
array, then print them one per line in reverse order.

**Constraints.**
- You don't know the count ahead of time: start with a small buffer and double its
  capacity with `realloc` when it fills.
- Use the temp-pointer idiom for `realloc`. Free the buffer before exiting.

**Verify.** `printf '1\n2\n3\n' | ./ex1` prints `3`, `2`, `1`. `valgrind` is clean.

---

## Exercise 2 — Repeat a string on the heap

**Goal.** Write `char *repeat(const char *s, size_t times)` that returns a new heap
string equal to `s` concatenated `times` times. The caller owns and frees it.

**Constraints.**
- Grow the output with `realloc` as you append each copy (temp-pointer idiom).
- Always keep the result NUL-terminated. Return `NULL` on allocation failure
  (and don't leak the partial buffer).

**Verify.** `repeat("ab", 3)` yields `"ababab"`. `valgrind` is clean after the
caller frees it.

---

## Exercise 3 — Fix the leak

**Goal.** `ex3_starter.c` runs correctly but leaks memory every loop iteration.
Fix it so output is unchanged and `valgrind` reports zero leaks.

**Constraints.**
- Don't change what the program prints.
- Change only what's needed to free every allocation exactly once.

**Verify.** Output is three `item-N` lines; `valgrind --leak-check=full ./ex3`
reports `definitely lost: 0 bytes`.

---

## Exercise 4 — Deep-copy an int array

**Goal.** Write `int *copy_array(const int *src, size_t n)` that returns a new heap
array holding copies of the `n` ints. Caller frees.

**Constraints.**
- Use `memcpy`. Check `malloc` for `NULL`.
- Prove it's a real copy: modify the original after copying and show the copy is
  unchanged.

**Verify.** Copy of `{1,2,3}`, then set `src[0] = 99`; the copy still starts with
`1`. `valgrind` is clean.

---

## Exercise 5 — Extend the vector with `pop` and `sum`

**Goal.** Starting from the `IntVec` in `ex5_starter.c`, implement:
- `bool vec_pop(IntVec *v, int *out)` — remove the last element, write it through
  `out`, return `true`; return `false` if the vector is empty.
- `long vec_sum(const IntVec *v)` — return the sum of all elements.

**Constraints.**
- `vec_pop` must not shrink the buffer (just decrement `len`); it must not read past
  the end when empty.
- Keep the init/push/free lifecycle intact and leak-free.

**Verify.** Push `1,2,3`; `vec_sum` is `6`; three pops yield `3,2,1`; a fourth pop
returns `false`. `valgrind` is clean.
