# Module 05 — Arrays & Strings

> C has no `list` and no `str`. It has fixed-size arrays and NUL-terminated
> `char` arrays. Master those — and their sharp edges — and you can read almost
> any C program.

## Where we are

Module 04 taught pointers and ended on the hint that `a[i]` is really `*(a + i)`.
This module cashes that in. Arrays *are* pointer arithmetic, and C strings are
just `char` arrays with a convention. The payoff is being able to handle text
without the buffer overflows that make the news.

## Learning objectives

By the end you can:

- Declare and use fixed-size arrays and recover their length with `sizeof`.
- Explain **array-to-pointer decay** and why a function can't learn an array's
  length from the array alone.
- State that indexing is arithmetic: `a[i] == *(a + i)`.
- Explain that a C string is a `char` array ended by `'\0'`.
- Use `<string.h>`: `strlen`, `strcpy`/`strncpy`, `strcmp`, `memcpy`, `strtok`,
  `strchr`/`strrchr`/`strstr`.
- Recognize the classic bugs (off-by-one, missing NUL, buffer overflow) and reach
  for the safe pattern instead.
- Parse command-line arguments from `argc`/`argv`.

---

## 1. Arrays: fixed size, contiguous, no attached length

```c
int scores[5] = {90, 80, 70, 60, 50};
```

That reserves 5 ints *side by side* in memory. Unlike a Python list, an array:

- has a **fixed size** chosen when you declare it — it never grows;
- stores elements **contiguously** (back to back), which is what makes pointer
  arithmetic work;
- does **not** remember its own length — you track that yourself.

Where the array is declared, you can compute the length with `sizeof`:

```c
#include <stdio.h>

int main(void) {
    int scores[5] = {90, 80, 70, 60, 50};
    size_t count = sizeof scores / sizeof scores[0];

    printf("the array holds %zu ints\n", count);
    printf("sizeof scores    = %zu bytes\n", sizeof scores);
    printf("sizeof scores[0] = %zu bytes\n", sizeof scores[0]);

    for (size_t i = 0; i < count; i++) {
        printf("scores[%zu] = %d\n", i, scores[i]);
    }
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/arrays_basics.c -o arrays_basics
$ ./arrays_basics
the array holds 5 ints
sizeof scores    = 20 bytes
sizeof scores[0] = 4 bytes
scores[0] = 90
scores[1] = 80
scores[2] = 70
scores[3] = 60
scores[4] = 50
```

`sizeof scores` is the whole array (20 bytes = 5 × 4); dividing by one element's
size gives the count. **This only works right here**, where the compiler can still
see the array type. Pass the array to a function and the trick breaks — next.

> **No bounds checking.** `scores[5]` or `scores[99]` compiles fine and reads/writes
> whatever happens to be there. There is no `IndexError`. Staying in bounds is
> *your* job — this is the root of most C security bugs.

---

## 2. Array-to-pointer "decay"

When you use an array name as a value — for example, pass it to a function — it
*decays* into a pointer to its first element. The length does not come along.

```c
#include <stdio.h>

void print_info(int *arr, size_t n) {
    printf("inside function, sizeof arr = %zu (just a pointer)\n", sizeof arr);
    printf("first element via arr[0] = %d\n", arr[0]);
    printf("caller told us n = %zu elements\n", n);
}

int main(void) {
    int nums[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    size_t count = sizeof nums / sizeof nums[0];

    printf("in main, sizeof nums = %zu (the whole array), count = %zu\n",
           sizeof nums, count);
    print_info(nums, count);
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/array_decay.c -o array_decay
$ ./array_decay
in main, sizeof nums = 40 (the whole array), count = 10
inside function, sizeof arr = 8 (just a pointer)
first element via arr[0] = 10
caller told us n = 10 elements
```

Even though the parameter is written `int arr[]` or `int *arr`, inside the function
it is a *pointer* (8 bytes), not an array. That is why **every C function that takes
an array also takes a length** (`n` here). Burn that in: pass the length.

---

## 3. Indexing is pointer arithmetic

`a[i]` is *defined* by the language as `*(a + i)`. Subscripting is syntactic sugar
over the pointer math from Module 04.

```c
#include <stdio.h>

int main(void) {
    char letters[4] = {'r', 'e', 'd', '!'};

    for (size_t i = 0; i < 4; i++) {
        printf("letters[%zu]=%c   *(letters+%zu)=%c\n",
               i, letters[i], i, *(letters + i));
    }

    printf("2[letters] = %c  (yes, really the same as letters[2])\n", 2[letters]);
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/index_as_arith.c -o index_as_arith
$ ./index_as_arith
letters[0]=r   *(letters+0)=r
letters[1]=e   *(letters+1)=e
letters[2]=d   *(letters+2)=d
letters[3]=!   *(letters+3)=!
2[letters] = d  (yes, really the same as letters[2])
```

Because `a + i` and `i + a` are the same, `a[i]` and `i[a]` are the same. You will
never *write* `2[letters]`, but it proves the point: indexing is arithmetic.

---

## 4. C strings: `char` arrays ending in `'\0'`

A C string is a block of `char` with a **NUL byte** (`'\0'`, value 0) marking the
end. There is no length stored anywhere — functions scan for the NUL to know where
to stop. Drop the NUL and those functions run off the end of your buffer.

```c
#include <stdio.h>

int main(void) {
    char word[6] = {'h', 'e', 'l', 'l', 'o', '\0'};
    char same[] = "hello";   /* compiler adds the '\0' -> 6 bytes */

    printf("word = %s\n", word);
    printf("same = %s\n", same);
    printf("sizeof the \"hello\" array = %zu (5 letters + 1 NUL)\n", sizeof same);

    for (size_t i = 0; i < sizeof same; i++) {
        if (same[i] == '\0') {
            printf("[%zu]=\\0 ", i);
        } else {
            printf("[%zu]=%c ", i, same[i]);
        }
    }
    putchar('\n');
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/string_basics.c -o string_basics
$ ./string_basics
word = hello
same = hello
sizeof the "hello" array = 6 (5 letters + 1 NUL)
[0]=h [1]=e [2]=l [3]=l [4]=o [5]=\0
```

The literal `"hello"` is 6 bytes, not 5 — the terminator is real and takes space.
**Always leave room for it** when you size a buffer.

---

## 5. `<string.h>`: the standard string toolkit

### `strlen` — length up to the NUL

```c
#include <stdio.h>
#include <string.h>

size_t my_strlen(const char *s) {
    size_t n = 0;
    while (s[n] != '\0') {
        n++;
    }
    return n;
}

int main(void) {
    const char *s = "redteam";
    printf("strlen    = %zu\n", strlen(s));
    printf("my_strlen = %zu\n", my_strlen(s));
    printf("the array needs %zu bytes (length + 1 for the NUL)\n", strlen(s) + 1);
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/strlen_demo.c -o strlen_demo
$ ./strlen_demo
strlen    = 7
my_strlen = 7
the array needs 8 bytes (length + 1 for the NUL)
```

`strlen` walks the string counting bytes — it is O(n), not free. Don't call it in a
loop condition over the same unchanging string.

### `strcpy` / `strncpy` — copying, and the missing-NUL trap

`strcpy(dst, src)` copies until it hits the NUL in `src`. If `src` is longer than
`dst`, it writes past the end — a **buffer overflow**. The bounded cousin
`strncpy(dst, src, n)` copies at most `n` bytes, but with a nasty twist: *if it hits
the limit first, it does not add a NUL*. You get an unterminated "string" — the
**missing-NUL** bug.

Two patterns that are always safe:

```c
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    const char *src = (argc > 1) ? argv[1] : "default-source-text";
    char dst[8];

    /* #1 snprintf: never overflows, ALWAYS NUL-terminates. */
    snprintf(dst, sizeof dst, "%s", src);
    printf("snprintf copy: \"%s\" (len %zu)\n", dst, strlen(dst));

    /* #2 strncpy with room for the terminator, then terminate by hand. */
    char dst2[8];
    strncpy(dst2, src, sizeof dst2 - 1);
    dst2[sizeof dst2 - 1] = '\0';
    printf("strncpy copy : \"%s\" (len %zu)\n", dst2, strlen(dst2));
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/string_copy.c -o string_copy
$ ./string_copy
snprintf copy: "default" (len 7)
strncpy copy : "default" (len 7)
$ ./string_copy supercalifragilistic
snprintf copy: "superca" (len 7)
strncpy copy : "superca" (len 7)
```

Both safely truncate the long input to 7 characters + NUL. My rule of thumb:
**prefer `snprintf` for building strings; if you use `strncpy`, terminate by hand
on the very next line.**

### `strcmp` — compare by content, never with `==`

```c
#include <stdio.h>
#include <string.h>

int main(void) {
    const char *a = "apple";
    const char *b = "apply";

    printf("strcmp(\"apple\", \"apple\") = %d\n", strcmp("apple", "apple"));
    printf("strcmp(\"apple\", \"apply\") = %d\n", strcmp(a, b));

    if (strcmp(a, b) == 0) {
        printf("equal\n");
    } else {
        printf("different\n");
    }
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/strcmp_demo.c -o strcmp_demo
$ ./strcmp_demo
strcmp("apple", "apple") = 0
strcmp("apple", "apply") = -1
different
```

`strcmp` returns **0 for equal** (not 1 — a common surprise). The sign tells you
order. Using `a == b` would compare *addresses*, which is almost never what you
mean when you're coming from Python's `==`.

### `memcpy` — copy raw bytes

```c
#include <stdio.h>
#include <string.h>

int main(void) {
    int src[5] = {1, 2, 3, 4, 5};
    int dst[5];

    memcpy(dst, src, sizeof src);   /* 20 bytes */

    for (size_t i = 0; i < 5; i++) {
        printf("%d ", dst[i]);
    }
    putchar('\n');
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/memcpy_demo.c -o memcpy_demo
$ ./memcpy_demo
1 2 3 4 5
```

`memcpy` knows nothing about NULs; it copies the byte count you give. It's for any
fixed block (arrays, structs). If the regions might overlap, use `memmove`.

### `strtok` — split on delimiters (and it edits your string)

```c
#include <stdio.h>
#include <string.h>

int main(void) {
    char line[] = "root:x:0:0:root:/root:/bin/bash";

    char *field = strtok(line, ":");
    int n = 0;
    while (field != NULL) {
        printf("field %d: %s\n", n++, field);
        field = strtok(NULL, ":");
    }
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/strtok_demo.c -o strtok_demo
$ ./strtok_demo
field 0: root
field 1: x
field 2: 0
field 3: 0
field 4: root
field 5: /root
field 6: /bin/bash
```

`strtok` writes NULs over the delimiters, so it **must** operate on a writable
array (never a string literal — that would crash or be undefined). The first call
passes the string; later calls pass `NULL` to say "continue where you left off."
You'll use exactly this to parse `/etc/passwd` lines in the capstone.

### `strchr` / `strrchr` / `strstr` — find a char or substring

```c
#include <stdio.h>
#include <string.h>

int main(void) {
    const char *path = "/usr/local/bin/scanner";

    const char *slash = strrchr(path, '/');
    if (slash != NULL) {
        printf("basename = %s\n", slash + 1);
    }

    const char *hit = strstr(path, "local");
    if (hit != NULL) {
        printf("found \"local\" at offset %td\n", hit - path);
    } else {
        printf("\"local\" not found\n");
    }
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/search_demo.c -o search_demo
$ ./search_demo
basename = scanner
found "local" at offset 5
```

Each returns a **pointer into the string** (or `NULL`). Subtracting that pointer
from the start gives the offset.

---

## 6. Reading input safely, and the bug that killed `gets`

The historical disaster `gets(buf)` read a line with **no size limit** — any input
longer than `buf` overran it. It was so unsafe it was *removed from the language* in
C11. Use `fgets`, which takes the buffer size:

```c
#include <stdio.h>
#include <string.h>

int main(void) {
    char buf[16];

    printf("type something and press enter:\n");
    if (fgets(buf, sizeof buf, stdin) == NULL) {
        printf("no input\n");
        return 0;
    }
    buf[strcspn(buf, "\n")] = '\0';   /* strip the trailing newline if present */

    printf("you typed %zu chars: \"%s\"\n", strlen(buf), buf);
    printf("(anything past %zu chars was safely left in the buffer)\n",
           sizeof buf - 1);
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/safe_input.c -o safe_input
$ echo "hello" | ./safe_input
type something and press enter:
you typed 5 chars: "hello"
(anything past 15 chars was safely left in the buffer)
$ echo "this line is definitely too long" | ./safe_input
type something and press enter:
you typed 15 chars: "this line is to"
(anything past 15 chars was safely left in the buffer)
```

`fgets` reads at most `sizeof buf - 1` bytes and always NUL-terminates. Oversized
input is left in the stream rather than smashing your stack.

---

## 7. Command-line arguments: `argc` and `argv`

`main` can take `int argc, char **argv`. `argv` is an array of strings: `argv[0]` is
the program name and `argv[1]`…`argv[argc-1]` are the arguments.

```c
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    printf("program name: %s\n", argv[0]);
    printf("got %d argument(s)\n", argc - 1);

    int verbose = 0;
    const char *target = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            target = argv[++i];
        } else {
            printf("unknown arg: %s\n", argv[i]);
        }
    }

    printf("verbose = %s\n", verbose ? "on" : "off");
    printf("target  = %s\n", target ? target : "(none)");
    return 0;
}
```

```console
$ gcc -std=c11 -Wall -Wextra examples/argv_demo.c -o argv_demo
$ ./argv_demo -v -t 10.0.0.5 extra
program name: ./argv_demo
got 4 argument(s)
unknown arg: extra
verbose = on
target  = 10.0.0.5
```

This little pattern — walk `argv`, match flags with `strcmp`, consume values — is
the skeleton of the capstone's option parser.

---

## Common mistakes / gotchas

- **Off-by-one on the NUL.** A buffer for an N-character string needs `N + 1` bytes.
  Writing N characters into an N-byte buffer leaves no room for `'\0'`.
- **Missing NUL from `strncpy`.** If it hits the length limit it does not terminate.
  Terminate by hand, or use `snprintf`.
- **Reading/writing out of bounds.** No bounds checks exist. `buf[i]` past the end is
  undefined behavior — maybe garbage, maybe a crash, maybe an exploitable overflow.
- **`==` on strings.** Compares addresses, not text. Use `strcmp(...) == 0`.
- **`strtok` on a literal.** It writes into the buffer; a string literal is read-only.
  Copy into a `char[]` first.
- **Expecting `sizeof` to work in a function.** It gives the pointer size there, not
  the array length. Pass the length separately.
- **Signed/unsigned loop counters.** Compare `size_t i` against a `size_t` length to
  avoid surprises and `-Wextra` warnings.

## Recap

- Arrays are fixed-size, contiguous, and carry no length; you track it yourself.
- Using an array as a value decays it to a pointer to its first element — so
  functions take a separate length argument.
- `a[i]` is `*(a + i)`: indexing is pointer arithmetic.
- A C string is a `char` array ended by `'\0'`; size buffers for length + 1.
- `<string.h>` gives you `strlen`, `strcpy`/`strncpy`, `strcmp`, `memcpy`, `strtok`,
  `strchr`/`strstr` — each with an edge you must respect.
- Prefer safe patterns (`snprintf`, `fgets`, bounds you enforce) to avoid overflows.
- Parse the command line with `argc`/`argv`.

## Exercises

See [`exercises/README.md`](exercises/README.md). Build everything with
`gcc -std=c11 -Wall -Wextra`.

## Further reading

- `man 3 string` (overview), `man 3 strncpy`, `man 3 strtok`, `man 3 snprintf`
- `man 3 fgets` — and note the warnings about the removed `gets`
- CWE-120 "Buffer Copy without Checking Size of Input" — why this module matters.
