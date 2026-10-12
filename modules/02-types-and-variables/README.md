# Module 02 — Types, Variables & Operators

*C's type system in depth: the base types, fixed-width integers, how integers and
floats really behave, the operators (bitwise included), and reading input safely.*

## Where we are

Module 01 showed that C is statically typed — every variable has one fixed type,
checked at compile time. This module is where you learn those types cold, because in C
the type you choose decides how much memory a value takes, what range it holds, and
exactly how arithmetic behaves. Get this wrong and you get silent corruption, not an
exception.

## Learning objectives

By the end you can:

- Name C's base types and choose `<stdint.h>` fixed-width types when size matters.
- Predict the result of signed/unsigned conversions, overflow, promotion, and
  truncation.
- Use the arithmetic, comparison, logical, and **bitwise** operators, and reason about
  precedence.
- Match `printf`/`scanf` format specifiers to types, and read user input safely with
  the `fgets` + `strtol` pattern.

## The base types

C's built-in numeric types, smallest to largest:

| Type | Typical size | Holds |
|---|---|---|
| `char` | 1 byte | a small integer / one character |
| `_Bool` | 1 byte | 0 or 1 (`#include <stdbool.h>` for `bool`, `true`, `false`) |
| `short` | 2 bytes | small integers |
| `int` | 4 bytes | the "default" integer |
| `long` | 8 bytes (Linux 64-bit) | larger integers |
| `long long` | 8 bytes | at least 64-bit integers |
| `float` | 4 bytes | single-precision floating point |
| `double` | 8 bytes | double-precision floating point |

The crucial caveat: **these sizes are not guaranteed.** The C standard only promises
minimum sizes and an ordering (`char ≤ short ≤ int ≤ long ≤ long long`). "`int` is 4
bytes" is true on normal Linux, but it is a property of your platform, not of C. When
you need to *know* the size, ask with `sizeof`, or use a fixed-width type.

[`examples/base_types.c`](examples/base_types.c) prints the real numbers for your
machine (`sizeof` gives a `size_t`, so we print it with `%zu`; ranges come from
`<limits.h>` and `<float.h>`):

```console
$ gcc -std=c11 -Wall -Wextra examples/base_types.c -o base_types
$ ./base_types
a byte on this machine is 8 bits (CHAR_BIT)

type           size (bytes)
char           1
_Bool          1
short          2
int            4
long           8
long long      8
float          4
double         8

some ranges:
int       : -2147483648 .. 2147483647
unsigned  : 0 .. 4294967295
long      : -9223372036854775808 .. 9223372036854775807
double max: 1.79769e+308
```

Unlike Python's arbitrary-precision `int` (which grows without limit), a C integer has
a hard ceiling fixed by its type. Exceed it and you do not get a bigger number — you
get overflow, discussed below.

## Fixed-width types: `<stdint.h>`

When the exact size is part of the contract — a field in a network packet, an offset in
a file format, a hardware register, a checksum — do not gamble on what `int` happens to
be. Use the fixed-width types from `<stdint.h>`:

| Signed | Unsigned | Bits | Range (unsigned) |
|---|---|---|---|
| `int8_t` | `uint8_t` | 8 | 0 .. 255 |
| `int16_t` | `uint16_t` | 16 | 0 .. 65 535 |
| `int32_t` | `uint32_t` | 32 | 0 .. ~4.29 billion |
| `int64_t` | `uint64_t` | 64 | 0 .. ~1.8×10¹⁹ |

The number in the name *is* the width in bits, everywhere, on every platform. To print
them portably, use the macros from `<inttypes.h>` (`PRId32`, `PRIu8`, …), which expand
to the correct length modifier. [`examples/fixed_width.c`](examples/fixed_width.c):

```console
$ gcc -std=c11 -Wall -Wextra examples/fixed_width.c -o fixed_width
$ ./fixed_width
uint8_t  byte   = 255  (1 bytes)
int16_t  small  = -30000  (2 bytes)
uint32_t medium = 4000000000  (4 bytes)
int64_t  big    = -9000000000  (8 bytes)
```

**Rule of thumb:** use plain `int` for ordinary counting and loop indices; reach for a
`uintN_t`/`intN_t` the moment a specific size matters. You will see these constantly
once we touch files, packets, and binary data.

## Signed vs. unsigned

Every integer type is either **signed** (can be negative) or **unsigned** (cannot;
spends that range on bigger positives). `int` is signed; prefix `unsigned` (or use a
`uintN_t`) for unsigned.

The dangerous part is conversion. Storing a negative value into an unsigned type does
not clamp to zero and does not raise anything — it **reinterprets the bits**, yielding a
large positive number. [`examples/signed_unsigned.c`](examples/signed_unsigned.c):

```console
$ gcc -std=c11 -Wall -Wextra examples/signed_unsigned.c -o signed_unsigned
$ ./signed_unsigned
(int)-1 stored as unsigned is 4294967295
(int)-1 stored as uint8_t is 255
a requested length of -4 becomes size_t 18446744073709551612
```

That last line is why this matters for security. Imagine code that reads a length,
forgets to reject a negative value, and passes it to a function expecting a `size_t`
size. A `-4` becomes ~18 quintillion — an enormous allocation or copy, and a classic
path to a buffer overflow. "Check for negative *before* converting to unsigned" is a
habit worth forming now.

One more trap: **comparing a signed and an unsigned value** converts the signed one to
unsigned first, so `-1 > 1U` is *true*. With `-Wextra`, `gcc` warns about this with
`-Wsign-compare` — heed it; do not silence it with a cast unless you have reasoned it
through.

## Overflow

What happens when a value runs past the top of its type?

- **Unsigned overflow is well-defined:** it wraps modulo 2ⁿ, like an odometer. `255 + 1`
  in a `uint8_t` is `0`.
- **Signed overflow is undefined behavior (UB).** The compiler is allowed to assume it
  never happens and optimize on that basis. Never rely on signed wraparound; the result
  is not "a negative number," it is "anything, including a miscompiled program."

[`examples/integer_overflow.c`](examples/integer_overflow.c) demonstrates only the
defined, unsigned case:

```console
$ gcc -std=c11 -Wall -Wextra examples/integer_overflow.c -o integer_overflow
$ ./integer_overflow
UINT_MAX     = 4294967295
UINT_MAX + 1 = 0   (wrapped to zero)

uint8_t counter = 255
after ++        = 0   (wrapped)

(uint8_t)(200 + 100) = 44   (300 wrapped past 255)
```

Python never does this — its ints grow forever. In C, overflow is always lurking; think
about the range of every value that could get large.

## Integer promotion and truncation

Two conversions happen so often they deserve names.

**Integer promotion:** before arithmetic, operands smaller than `int` (`char`, `short`,
`_Bool`) are first converted to `int`. So `char + char` is computed as `int`.
[`examples/integer_promotion.c`](examples/integer_promotion.c):

```console
$ gcc -std=c11 -Wall -Wextra examples/integer_promotion.c -o integer_promotion
$ ./integer_promotion
a = 100, b = 100
a + b = 200
sizeof(a)     = 1
sizeof(a + b) = 4
```

`a + b` is `200` even though a `char` tops out around `127`, because the addition
happens in `int`. And `sizeof(a + b)` is `4` — the expression's type is `int`, not
`char`. This is usually helpful, but it surprises people who expect `char` math to stay
in a `char`.

**Truncation:** moving a value into a type too small (or into an integer from a float)
drops information. [`examples/truncation.c`](examples/truncation.c) uses explicit casts
to make the narrowing intentional:

```console
$ gcc -std=c11 -Wall -Wextra examples/truncation.c -o truncation
$ ./truncation
int 1000 narrowed to uint8_t = 232
(int)3.99 = 3   (fraction dropped, not rounded)
(int)-2.7 = -2   (toward zero, so -2)
```

`1000` keeps only its low 8 bits (`1000 mod 256 = 232`). Float-to-int **truncates toward
zero** — it does not round, so `3.99` becomes `3` and `-2.7` becomes `-2`. When you
genuinely intend to narrow, write the cast `(uint8_t)` or `(int)` so it is visible; an
accidental narrowing is a bug hiding in plain sight.

## Operators and precedence

Arithmetic (`+ - * / %`), comparison (`== != < <= > >=`), and logical (`&& || !`)
operators are mostly what you expect, with two C-specific surprises for a Python
programmer:

1. **Integer `/` is integer division.** There is no separate `//`. `7 / 2` is `3`. To
   get `3.5`, make an operand a float: `7.0 / 2`.
2. **Comparisons and logicals produce `int`s `0` or `1`**, not a dedicated bool. `x > 0`
   evaluates to `1` or `0`.

[`examples/operators.c`](examples/operators.c):

```console
$ gcc -std=c11 -Wall -Wextra examples/operators.c -o operators
$ ./operators
7 / 2    = 3   (integer division: quotient only)
7 % 2    = 1   (modulo: the remainder)
7.0 / 2  = 3.5 (floating division: one operand is a double)

2 + 3 * 4   = 14   (* first)
(2 + 3) * 4 = 20   (parentheses first)

(x > 0) is 1
(x > 0 && x < 10) is 1
!(x == 5) is 0
```

**Precedence** decides what binds first when you omit parentheses: `*` and `/` before
`+` and `-`; arithmetic before comparisons before `&&` before `||`. You do not need to
memorize the full 15-level table — when in doubt, **add parentheses.** They cost nothing
and make intent unambiguous. `&&` and `||` also **short-circuit**: `a && b` never
evaluates `b` if `a` is false, which you will rely on to guard against, say,
dereferencing a null pointer.

## Bitwise operators

These operate on the individual bits of integer values. They are essential later — for
permission flags, packing data, protocol fields, and reading `/proc` and file-mode
bits — so learn them now.

| Op | Name | Effect | Typical use |
|---|---|---|---|
| `&` | AND | 1 where **both** bits are 1 | test or clear bits with a mask |
| `\|` | OR | 1 where **either** bit is 1 | set bits |
| `^` | XOR | 1 where bits **differ** | toggle bits |
| `~` | NOT | flips every bit | build a "clear" mask |
| `<<` | left shift | shift bits left, zero-fill | multiply by 2ⁿ; build masks |
| `>>` | right shift | shift bits right | divide by 2ⁿ |

Do bitwise work on **unsigned** types — shifting around the sign bit of a signed value
leads to surprises and UB. A single bit is selected with a shifted `1`: `(1u << 3)` is
the mask for bit 3. [`examples/bitwise.c`](examples/bitwise.c) prints the binary so you
can see each operation, then shows the set/test/toggle/clear flag idiom:

```console
$ gcc -std=c11 -Wall -Wextra examples/bitwise.c -o bitwise
$ ./bitwise
x        = 00001111  (0x0F)
y        = 00110011  (0x33)
x & y    = 00000011
x | y    = 00111111
x ^ y    = 00111100
~x       = 11110000
x << 1   = 00011110
x >> 2   = 00000011

perms after set R,W      = 00000011
has WRITE? 1
has EXEC?  0
perms after toggle WRITE = 00000001
perms after clear READ   = 00000000
```

The four idioms worth memorizing (where `MASK` is a single-bit value like `1u << n`):

```c
flags |=  MASK;    /* set   the bit(s) in MASK            */
flags &= ~MASK;    /* clear the bit(s) in MASK            */
flags ^=  MASK;    /* toggle the bit(s) in MASK           */
if (flags & MASK)  /* test: non-zero means the bit is set */
```

This is exactly how Unix file permissions (`rwx`), among countless other things, are
represented. You will use it in the capstone to interpret file modes.

## `printf`/`scanf` format specifiers

`printf` does not know the types of its arguments at runtime — it *believes* the format
string. A mismatch (e.g. `%d` for a `double`) is undefined behavior. With `-Wall
-Wextra`, `gcc` cross-checks the format string against the arguments, which catches most
mistakes at compile time. The common specifiers:

| Specifier | Argument type | Notes |
|---|---|---|
| `%d` | `int` | signed decimal |
| `%u` | `unsigned int` | unsigned decimal |
| `%ld` / `%lld` | `long` / `long long` | length modifiers `l`, `ll` |
| `%f` | `double` | default 6 decimals; `%.2f` for 2 |
| `%e` / `%g` | `double` | scientific / compact |
| `%c` | `int` (a character) | one character |
| `%s` | `char *` | a NUL-terminated string |
| `%x` / `%o` | `unsigned int` | hex / octal |
| `%p` | `void *` | a pointer; cast with `(void *)` |
| `%zu` | `size_t` | for `sizeof` results and sizes |
| `%%` | — | a literal `%` |

[`examples/printf_formats.c`](examples/printf_formats.c):

```console
$ gcc -std=c11 -Wall -Wextra examples/printf_formats.c -o printf_formats
$ ./printf_formats
%d  signed int      : -42
%u  unsigned int    : 42
%ld long            : 1234567890
%f  double          : 3.141590
%.2f fixed decimals : 3.14
%e  scientific      : 3.141590e+00
%g  compact float   : 3.14159
%c  single char     : Z
%s  string          : hello
%x  hex (lower)     : dead
%o  octal           : 52
%zu size_t          : 4
%p  pointer         : 0x7ffd... (address varies)
literal percent sign : %
```

(The pointer address differs every run — that is expected.)

## Reading input safely

`scanf` exists and looks convenient, but it is a frequent source of bugs: `scanf("%d",
&n)` leaves the rest of the line (including the newline) sitting in the input buffer,
silently fails on non-numeric input, and `scanf("%s", buf)` has no idea how big `buf` is
— a textbook buffer overflow. Build a better habit from day one:

**Read a whole line with `fgets`, then parse it with `strtol`.** `fgets` takes the
buffer size, so it cannot overflow. `strtol` tells you where parsing stopped (via an
`end` pointer) and signals overflow (via `errno`), so you can reject bad input cleanly.
[`examples/safe_input.c`](examples/safe_input.c):

```console
$ gcc -std=c11 -Wall -Wextra examples/safe_input.c -o safe_input
$ echo 123 | ./safe_input
enter an integer: you entered 123; doubled it is 246
$ echo abc | ./safe_input
enter an integer: that was not a number
$ echo 99999999999999999999 | ./safe_input
enter an integer: number out of range
```

The checks — `end == line` (no digits at all), `errno == ERANGE` (too big), and trailing
junk after optional whitespace — are the difference between a toy and a program that
does not fall over on the first unexpected keystroke. We lean on this pattern whenever a
program takes numeric input.

## Common mistakes / gotchas

- **Assuming `int` is 4 bytes everywhere.** It usually is on Linux, but use `sizeof` or
  fixed-width types when it matters.
- **Negative into unsigned.** `-1` becomes a huge positive value, not an error. Validate
  before converting, especially for sizes and lengths.
- **Relying on signed overflow.** It is undefined behavior. Use unsigned types if you
  actually want wraparound.
- **Integer division surprise.** `1 / 2` is `0`, not `0.5`. Make an operand a float, or
  you will lose the fraction. `avg = sum / count` with int operands truncates.
- **Mismatched `printf` specifier.** `%d` for a `long`, or `%f` for an `int`, is UB. Let
  `-Wall -Wextra` catch these.
- **Using `%s` or `%d` with `scanf` carelessly.** Prefer `fgets` + `strtol`; never
  `scanf("%s", buf)` without a width limit.
- **Forgetting `&` with `scanf`.** If you ever do use `scanf`, it needs the *address*
  (`&n`). (Pointers are Module 04 — another reason to prefer `fgets`+`strtol` for now.)

## Recap

- Base types have platform-dependent sizes with guaranteed minimums; `sizeof` reports
  the truth, and `<stdint.h>` gives you exact widths when size is part of the contract.
- **Signed→unsigned** conversion reinterprets bits (negatives become huge); validate
  before converting sizes.
- **Unsigned overflow wraps; signed overflow is undefined.** Integer promotion computes
  small-type arithmetic as `int`; narrowing truncates — cast when you mean it.
- `/` between integers is integer division; `&& ||` short-circuit; when unsure about
  precedence, **parenthesize**.
- Master the four **bitwise** flag idioms — set/clear/toggle/test — you will use them for
  the rest of the course.
- Match `printf` specifiers to types, and read input with **`fgets` + `strtol`**, not
  bare `scanf`.

## Exercises

See [`exercises/README.md`](exercises/README.md).

## Further reading

- `man 3 printf` and `man 3 scanf` — every conversion specifier and its pitfalls.
- `man 3 strtol` — the robust string-to-number converter and its error reporting.
- `man 7 stdint` (or the header itself) — the fixed-width types.
- *Secure Coding in C and C++* (Seacord), chapter on integers — why these rules are
  security-critical.
