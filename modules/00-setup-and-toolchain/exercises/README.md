# Module 00 — Exercises

Build everything with:

```bash
gcc -std=c11 -Wall -Wextra FILE.c -o PROG
```

Try each yourself before peeking at [`../solutions/`](../solutions/).

---

## Exercise 1 — Greet yourself, exit cleanly

**Goal.** Write `ex1.c` that prints a greeting containing your name on its own line,
then returns success.

**Constraints.**
- Use `printf` and `#include <stdio.h>`.
- End the printed line with `\n`.
- Return the success exit code from `main`.

**Verify.**
```bash
gcc -std=c11 -Wall -Wextra ex1.c -o ex1
./ex1
echo $?        # must print 0
```
The build must be silent (no warnings).

---

## Exercise 2 — A chosen exit code

**Goal.** Write `ex2.c` that prints one line of text and then returns the exit code
**42**.

**Constraints.**
- The number 42 must be what the shell sees in `$?`.

**Verify.**
```bash
gcc -std=c11 -Wall -Wextra ex2.c -o ex2
./ex2 ; echo $?   # must print 42
```

Bonus: return `300` instead and explain (in a comment) why `$?` shows `44`.
(Hint: exit codes are one byte; 300 wraps modulo 256.)

---

## Exercise 3 — Silence the warnings

**Goal.** Start from [`ex3_starter.c`](ex3_starter.c). As given, it builds but emits
warnings. Fix the code so the build is completely clean, without changing what it is
clearly trying to do (print a computed value).

**Constraints.**
- Do not suppress warnings with flags — fix the actual code.
- Keep `-Wall -Wextra` on.

**Verify.**
```bash
gcc -std=c11 -Wall -Wextra ex3.c -o ex3   # zero warnings
./ex3
```

Read each warning top to bottom and fix the first one first. There are two problems:
a variable used before it has a value, and a parameter the program never uses.
