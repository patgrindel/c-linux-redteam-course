# Module 02 — Exercises

Standard build:

```bash
gcc -std=c11 -Wall -Wextra FILE.c -o PROG
```

Every solution must build with **zero** warnings. Try each before checking
[`../solutions/`](../solutions/).

---

## Exercise 1 — Type explorer

**Goal.** Write `ex1.c` that prints, for `uint8_t`, `uint16_t`, `uint32_t`, and
`int32_t`: the size in bytes and the maximum value each can hold.

**Constraints.**
- Include `<stdint.h>` and `<inttypes.h>`; print values with the `PRI*` macros.
- Get the maxima from `<stdint.h>` limit macros (`UINT8_MAX`, `UINT16_MAX`,
  `UINT32_MAX`, `INT32_MAX`) — do not hard-code the numbers.
- Print sizes with `%zu`.

**Verify.**
```bash
gcc -std=c11 -Wall -Wextra ex1.c -o ex1
./ex1
```
Confirm `uint8_t` max is `255`, `uint16_t` max is `65535`, and sizes are 1/2/4/4.

---

## Exercise 2 — Watch a counter wrap

**Goal.** Write `ex2.c` with a `uint8_t` counter starting at `250`. In a loop, add 1
ten times; each iteration print the value. Show that it wraps past 255 back to 0.

**Constraints.**
- The counter must be `uint8_t` (so the wrap is well-defined).
- Print with `PRIu8`.

**Verify.**
```bash
gcc -std=c11 -Wall -Wextra ex2.c -o ex2
./ex2
```
The printed sequence must go `... 254 255 0 1 ...`.

---

## Exercise 3 — Permission flags

**Goal.** Starting from [`ex3_starter.c`](ex3_starter.c) (which gives you a
`print_bits_u8` helper and three flag macros), implement a `uint8_t perms` that you:
set to READ|WRITE, test for each flag, toggle EXEC on, then clear WRITE — printing the
binary after each step.

**Constraints.**
- Use only the bitwise idioms (`|=`, `&=`, `^=`, `&`, `~`); no `if`-ladder hacks.
- Test results printed as `0`/`1`.

**Verify.**
```bash
gcc -std=c11 -Wall -Wextra ex3.c -o ex3
./ex3
```
After set R|W the byte is `00000011`; after toggling EXEC on it is `00000111`; after
clearing WRITE it is `00000101`.

---

## Exercise 4 — Sum two inputs safely

**Goal.** Write `ex4.c` that reads **two** lines, each an integer, using the
`fgets` + `strtol` pattern, and prints their sum. Reject non-numeric or
out-of-range input with a clear message and a non-zero exit code.

**Constraints.**
- No bare `scanf`. Use `fgets` into a fixed buffer and `strtol`.
- Validate each line: digits present, no trailing junk, in `int` range.

**Verify.**
```bash
gcc -std=c11 -Wall -Wextra ex4.c -o ex4
printf '20\n22\n' | ./ex4     # expect a line reporting the sum is 42
printf 'x\n5\n'   | ./ex4     # expect an error, non-zero exit
echo $?
```

---

## Exercise 5 — Decimal to binary

**Goal.** Write `ex5.c` that reads one integer in the range 0–255 (via `fgets` +
`strtol`) and prints its 8-bit binary representation, using bitwise operations.

**Constraints.**
- Reject input outside 0–255.
- Build the binary string with shifts and masks, not with division loops or
  library helpers.

**Verify.**
```bash
gcc -std=c11 -Wall -Wextra ex5.c -o ex5
echo 5   | ./ex5    # expect 00000101
echo 255 | ./ex5    # expect 11111111
echo 300 | ./ex5    # expect an out-of-range error
```
