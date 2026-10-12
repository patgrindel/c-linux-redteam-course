# Module 06 — Exercises

Build with `gcc -std=c11 -Wall -Wextra exN.c -o exN`. No warnings. Try before
peeking at `solutions/`.

---

## Exercise 1 — A Color struct

**Goal.** Define `typedef struct { int r; int g; int b; } Color;` and a function
`int brightness(Color c)` returning the average of the three channels.

**Constraints.**
- Pass the `Color` by value (it's small).
- Use designated initializers in `main` to build at least one color.

**Verify.** `brightness((Color){.r=90, .g=90, .b=90})` is `90`; a pure-red
`{255,0,0}` is `85`. Starter: `ex1_starter.c`.

---

## Exercise 2 — Count privileged ports

**Goal.** Given an array of findings (IP + port), write
`int count_privileged(const Finding *f, size_t n)` that returns how many have a
port below 1024 (the "privileged" range worth noting on a host you're assessing).

**Constraints.**
- Define `Finding` with at least `const char *ip;` and `int port;`.
- Pass the array and its length; use `const` and `size_t`.

**Verify.** For ports `{22, 80, 8080, 443, 3389}` the answer is `3`.

---

## Exercise 3 — Traffic-light state machine

**Goal.** Define `typedef enum { LIGHT_RED, LIGHT_GREEN, LIGHT_YELLOW } Light;` and
`Light next_light(Light l)` returning the next state: RED→GREEN→YELLOW→RED.

**Constraints.**
- Implement it with a `switch`.
- Make sure the function returns a value on every path (add a final `return`).

**Verify.** Starting at RED and stepping 4 times prints RED GREEN YELLOW RED GREEN.

---

## Exercise 4 — Sum the ints in a tagged union

**Goal.** Reuse the tagged-union idea: a `Value` is either an int or a text string.
Write `int sum_ints(const Value *vals, size_t n)` that adds up only the `VAL_INT`
values and ignores the text ones.

**Constraints.**
- Define the `ValueKind` enum, the `Value` tagged union, and check the tag before
  reading a member.

**Verify.** For `{int 10, text "skip", int 5, int 7}` the sum is `22`.

---

## Exercise 5 — Shrink a struct by reordering

**Goal.** You're given `struct wasteful { char a; double b; char c; int d; };`.
Declare a second struct `struct tight` with the **same fields reordered** to
minimize padding, and print both sizes.

**Constraints.**
- Don't remove or change any field — only reorder.
- Print both sizes with `%zu`.

**Verify.** `sizeof(struct tight) <= sizeof(struct wasteful)` (on x86-64, typically
24 vs 16). Explain the difference in a comment.
