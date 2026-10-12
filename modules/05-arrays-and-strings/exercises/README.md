# Module 05 — Exercises

Build with `gcc -std=c11 -Wall -Wextra exN.c -o exN`. No warnings. Try before
peeking at `solutions/`.

---

## Exercise 1 — Count a character

**Goal.** Write `size_t count_char(const char *s, char c)` that returns how many
times `c` appears in `s`.

**Constraints.**
- Use `const` and `size_t`.
- Walk the string yourself (a loop to the NUL); don't call library search functions.

**Verify.** `count_char("mississippi", 's')` is `4`; `count_char("abc", 'z')` is `0`.
Starter: `ex1_starter.c`.

---

## Exercise 2 — A safe bounded copy

**Goal.** Write `void safe_copy(char *dst, size_t dstsize, const char *src)` that
copies `src` into `dst`, never writing more than `dstsize` bytes, and **always**
NUL-terminates (as long as `dstsize > 0`).

**Constraints.**
- Do not call `strcpy`/`strncpy`/`snprintf` — write the loop yourself so you
  understand the bound.
- If `dstsize == 0`, do nothing.

**Verify.** Copying `"supercalifragilistic"` into an 8-byte buffer yields
`"superca"` (7 chars + NUL) and never overflows.

---

## Exercise 3 — Reverse a string in place

**Goal.** Write `void reverse(char *s)` that reverses the characters of `s` in place.

**Constraints.**
- Modify the buffer directly; allocate nothing.
- Use `strlen` once to get the length, then swap from both ends inward.
- Remember: the input must be a writable `char[]`, not a string literal.

**Verify.** `"redteam"` becomes `"maetder"`; a 1-char and empty string are unchanged.

---

## Exercise 4 — Count words with `strtok`

**Goal.** Count the whitespace-separated words in a sentence using `strtok`.

**Constraints.**
- Delimiters are space, tab, and newline (`" \t\n"`).
- `strtok` modifies its input, so copy the sentence into a writable `char[]` first.

**Verify.** `"  the  quick brown  fox "` reports `4` words.

---

## Exercise 5 — Parse a `-n COUNT` flag

**Goal.** A program `./ex5 -n 3 hello` that prints its last argument `COUNT` times,
one per line. Default `COUNT` is 1 if `-n` is absent.

**Constraints.**
- Parse `argv` yourself; match `-n` with `strcmp` and read the next arg as the count
  (use `atoi` or `strtol`).
- Ignore a missing or non-positive count gracefully (treat as 1).

**Verify.** `./ex5 -n 3 hi` prints `hi` three times; `./ex5 hi` prints it once.
