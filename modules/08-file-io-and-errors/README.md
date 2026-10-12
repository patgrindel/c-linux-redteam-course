# Module 08 — File I/O & Error Handling

Reading and writing files the C way, with two different APIs — and the error-checking
discipline that separates working C from C that silently corrupts your data.

## Where we are

In Module 07 you learned to allocate and own memory on the heap. Now you'll pull real
data *into* that memory — from files on disk — and write results back out. This is also
where we get serious about **errors**: in C, nothing throws an exception; every call that
can fail hands you a value you must check.

## Learning objectives

By the end you can:

- Use buffered **streams** (`FILE *`: `fopen`, `fgets`, `fread`, `fprintf`, `fclose`).
- Use raw **file descriptors** (`open`, `read`, `write`, `close`) and explain how they
  differ from streams and when to reach for each.
- Read errno, and turn it into a human message with `perror` and `strerror`.
- Apply the "check every call" discipline so failures surface immediately.
- Read a file line by line and parse structured text — using `/etc/passwd` as the running
  example (you'll reuse this in the capstone).

---

## Two APIs for the same job

Python gives you one obvious way to read a file:

```python
with open("/etc/passwd") as f:
    for line in f:
        print(line.strip())
```

C gives you **two** layers, and it matters that you know both:

| Layer | Type | Functions | Buffered? | Defined by |
|-------|------|-----------|-----------|------------|
| Streams (stdio) | `FILE *` | `fopen`, `fgets`, `fread`, `fwrite`, `fprintf`, `fclose` | Yes, in userspace | C standard |
| File descriptors | `int` | `open`, `read`, `write`, `close` | No (each call is a syscall) | POSIX / the OS |

A **stream** (`FILE *`) is a library object wrapping a file descriptor plus an in-memory
buffer. A **file descriptor** is just a small non-negative integer — an index into the
kernel's per-process table of open files. `0`, `1`, `2` are always stdin, stdout, stderr.

Rule of thumb: use **streams** for ordinary text and formatted I/O (it's simpler and
faster for small reads/writes because it batches them). Drop to **file descriptors** when
you need exact control, when you're talking to things that aren't files (pipes, sockets —
Modules 09 and 10), or when a syscall interface demands an `int` fd.

---

## Streams: `fopen` / `fgets` / `fprintf` / `fclose`

`fopen(path, mode)` returns a `FILE *`, or `NULL` on failure. The mode is a string:
`"r"` read, `"w"` truncate+write, `"a"` append, `"r+"` read/write, add `"b"` for binary.

[`examples/fopen_cat.c`](examples/fopen_cat.c) — read a file line by line and echo it,
like a tiny `cat`:

```c
#include <stdio.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {                 /* ALWAYS check fopen */
        perror(argv[1]);             /* prints: "<path>: No such file or directory" */
        return 1;
    }

    char line[256];
    /* fgets stops at a newline OR when the buffer is nearly full; it keeps the '\n'.
       It returns NULL at end-of-file or on error, which ends the loop. */
    while (fgets(line, sizeof line, f) != NULL) {
        fputs(line, stdout);
    }

    if (ferror(f)) {                 /* distinguish a read error from clean EOF */
        fprintf(stderr, "error reading %s\n", argv[1]);
        fclose(f);
        return 1;
    }

    fclose(f);
    return 0;
}
```

Build and run:

```
$ gcc -std=c11 -Wall -Wextra examples/fopen_cat.c -o fopen_cat
$ ./fopen_cat /etc/hostname
mybox
$ ./fopen_cat /nope
/nope: No such file or directory
```

Key points the Python habit misses:

- **`fgets` keeps the trailing `\n`.** If you compare the line to something, strip it
  first (`line[strcspn(line, "\n")] = '\0';`).
- **You must size the buffer.** If a line is longer than `sizeof line - 1`, `fgets` gives
  you a partial line and the rest comes next call. For unbounded lines use `getline`
  (below).
- **`fopen` can fail** — bad path, no permission, too many open files. `NULL` means stop.

### Formatted output: `fprintf`

`fprintf(stream, fmt, ...)` is `printf` aimed at any stream. `printf(...)` is exactly
`fprintf(stdout, ...)`. Send errors and diagnostics to `stderr` so they don't pollute
piped output:

```c
fprintf(stderr, "warning: skipping malformed line %d\n", n);
```

### `getline` — read a whole line, however long

`fgets` needs a fixed buffer. POSIX `getline` grows one for you with `malloc`/`realloc`,
so you never truncate. It needs the `_POSIX_C_SOURCE` feature-test macro because it is not
part of ISO C11 — without it, `-std=c11` hides the declaration and you'd get an implicit-
declaration warning.

[`examples/read_lines.c`](examples/read_lines.c):

```c
#define _POSIX_C_SOURCE 200809L   /* expose getline() under -std=c11 */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        perror(argv[1]);
        return 1;
    }

    char *line = NULL;       /* getline allocates/reallocs this for us */
    size_t cap = 0;          /* current buffer capacity */
    ssize_t len;             /* bytes read, or -1 at EOF/error */
    long n = 0;

    while ((len = getline(&line, &cap, f)) != -1) {
        n++;
        /* strip the trailing newline if present */
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        printf("%4ld: %s\n", n, line);
    }

    free(line);              /* getline's buffer is ours to free */
    fclose(f);
    return 0;
}
```

```
$ gcc -std=c11 -Wall -Wextra examples/read_lines.c -o read_lines
$ printf 'alpha\nbeta\ngamma\n' > /tmp/three.txt
$ ./read_lines /tmp/three.txt
   1: alpha
   2: beta
   3: gamma
```

`getline` returns `ssize_t` (a signed size) so it can return `-1`. You own `line` and must
`free` it exactly once, after the loop — not inside it (it's reused each iteration).

---

## File descriptors: `open` / `read` / `write` / `close`

One level down, you work with raw descriptors. There is no buffering: each `read` and
`write` is a system call into the kernel (you'll watch these with `strace` in Module 09).

```c
#include <fcntl.h>      /* open, O_* flags */
#include <unistd.h>     /* read, write, close */
```

- `int fd = open(path, flags, mode);` — `flags` is a bitwise-OR: `O_RDONLY`, `O_WRONLY`,
  `O_RDWR`, plus `O_CREAT`, `O_TRUNC`, `O_APPEND`, … Returns `-1` on failure.
- `ssize_t n = read(fd, buf, count);` — up to `count` bytes. Returns the number actually
  read (**may be fewer** than asked, even for files), `0` at end of file, `-1` on error.
- `ssize_t n = write(fd, buf, count);` — returns bytes written (**may be fewer**!), `-1` on
  error. You must loop to write everything.
- `close(fd);` — releases the descriptor. Returns `-1` on error (worth checking when
  writing, since buffered data may flush here).

See `open(2)`, `read(2)`, `write(2)`, `close(2)`.

[`examples/fd_cat.c`](examples/fd_cat.c) — `cat` using descriptors, with a correct
partial-write loop:

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

/* Write the whole buffer, looping because write() may do it in pieces. */
static int write_all(int fd, const char *buf, size_t count) {
    size_t off = 0;
    while (off < count) {
        ssize_t n = write(fd, buf + off, count - off);
        if (n < 0) {
            return -1;          /* caller inspects errno */
        }
        off += (size_t) n;
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror(argv[1]);
        return 1;
    }

    char buf[4096];
    ssize_t n;
    while ((n = read(fd, buf, sizeof buf)) > 0) {
        if (write_all(STDOUT_FILENO, buf, (size_t) n) < 0) {
            perror("write");
            close(fd);
            return 1;
        }
    }
    if (n < 0) {                 /* read error, not EOF */
        perror(argv[1]);
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}
```

```
$ gcc -std=c11 -Wall -Wextra examples/fd_cat.c -o fd_cat
$ echo "hello from a descriptor" > /tmp/hi.txt
$ ./fd_cat /tmp/hi.txt
hello from a descriptor
```

`STDOUT_FILENO` (from `<unistd.h>`) is just the constant `1`. Note the `write_all` loop:
a bare `write(fd, buf, n)` is a classic bug — on a pipe or socket it can write fewer bytes
than requested and you'd silently drop the rest.

### Streams vs. descriptors, concretely

- `FILE *f = fopen(...)` buffers in userspace; `fgets`/`fwrite` may not touch the kernel at
  all until the buffer fills or you `fflush`/`fclose`. Great for many small ops.
- `int fd = open(...)` is unbuffered; every `read`/`write` is a syscall. Great for control
  and for non-file objects.
- You can convert: `fileno(f)` gets the fd under a stream; `fdopen(fd, mode)` wraps a stream
  around an fd. Don't freely mix reads/writes on both for the same open file — the buffers
  get out of sync.

---

## Errors: `errno`, `perror`, `strerror`

When a C library or system call fails, it almost always:

1. returns a sentinel value (`NULL`, or `-1`, or `EOF`), **and**
2. sets the global `errno` (from `<errno.h>`) to a numeric error code.

`errno` is only meaningful **right after** a call that failed — a later successful call may
not reset it. So check the return value first, read `errno` immediately.

- `perror("context")` prints `context: <message for errno>` to stderr.
- `strerror(errno)` returns the message as a string (useful with `fprintf`).

[`examples/errno_demo.c`](examples/errno_demo.c):

```c
#include <errno.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    FILE *f = fopen("/root/secret.txt", "r");   /* you almost certainly can't read this */
    if (f == NULL) {
        /* Two equivalent ways to report the same failure: */
        perror("open /root/secret.txt");
        fprintf(stderr, "errno=%d: %s\n", errno, strerror(errno));
        return 1;
    }
    fclose(f);
    return 0;
}
```

```
$ gcc -std=c11 -Wall -Wextra examples/errno_demo.c -o errno_demo
$ ./errno_demo
open /root/secret.txt: Permission denied
errno=13: Permission denied
```

(If you *are* root, you'll see "No such file or directory" instead — the point is the
same: the call told you exactly why it failed.)

Common codes worth recognizing: `ENOENT` (no such file), `EACCES` (permission denied),
`EISDIR` (it's a directory), `EMFILE` (too many open files), `EINTR` (interrupted by a
signal — Module 09). See `errno(3)` for the full list.

### The "check every call" discipline

This is the single most important habit in C systems programming. Every function that
does I/O or allocation **can** fail, and C will not stop you from using a garbage result.

Do **not** write:

```c
FILE *f = fopen(path, "r");
fgets(buf, sizeof buf, f);     /* if fopen failed, f is NULL -> crash */
```

Do write:

```c
FILE *f = fopen(path, "r");
if (f == NULL) {
    perror(path);
    return 1;
}
```

The defensive framing matters for the rest of this course: when you're assessing a host
you're authorized to test, "permission denied" vs. "no such file" on a config file is a
*finding*, not noise. Propagating the real error up is how your capstone tool will tell
the difference.

---

## Worked example: parsing `/etc/passwd`

`/etc/passwd` is world-readable and lists every account on the system. Each line is seven
colon-separated fields:

```
root:x:0:0:root:/root:/bin/bash
name:passwd:UID:GID:GECOS:home_dir:shell
```

We'll read it line by line and split each line on `:`. A subtlety: **fields can be empty**
(e.g. the GECOS field), which produces two colons in a row (`::`). The classic tool for
splitting, `strtok`, *collapses* consecutive delimiters and would misalign the fields.
So we split by hand with `strchr`, which also keeps the lesson honest about pointers.

[`examples/parse_passwd.c`](examples/parse_passwd.c):

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NFIELDS 7

/* Split `line` in place on ':' into up to NFIELDS pieces.
   Writes pointers into out[]; returns the field count.
   Modifies `line` by replacing ':' with '\0' (standard in-place parse). */
static int split_fields(char *line, char *out[], int max) {
    int n = 0;
    char *start = line;
    for (char *p = line; ; p++) {
        if (*p == ':' || *p == '\0') {
            int last = (*p == '\0');
            *p = '\0';
            if (n < max) {
                out[n++] = start;
            }
            start = p + 1;
            if (last) {
                break;
            }
        }
    }
    return n;
}

int main(int argc, char **argv) {
    const char *path = (argc == 2) ? argv[1] : "/etc/passwd";

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        perror(path);
        return 1;
    }

    char *line = NULL;
    size_t cap = 0;
    ssize_t len;
    int lineno = 0;

    while ((len = getline(&line, &cap, f)) != -1) {
        lineno++;
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';          /* drop newline */
        }
        if (line[0] == '\0' || line[0] == '#') {
            continue;                       /* skip blanks and comments */
        }

        char *fields[NFIELDS];
        int n = split_fields(line, fields, NFIELDS);
        if (n != NFIELDS) {
            fprintf(stderr, "%s:%d: expected %d fields, got %d (skipping)\n",
                    path, lineno, NFIELDS, n);
            continue;
        }

        /* fields: 0=name 1=passwd 2=uid 3=gid 4=gecos 5=home 6=shell */
        char *end;
        long uid = strtol(fields[2], &end, 10);
        if (*end != '\0') {
            fprintf(stderr, "%s:%d: bad UID '%s'\n", path, lineno, fields[2]);
            continue;
        }

        /* Report "real" user accounts (UID >= 1000) and root — the kind of
           triage you do when assessing who can log in to a host. */
        if (uid == 0 || uid >= 1000) {
            printf("%-16s uid=%-6ld shell=%s\n", fields[0], uid, fields[6]);
        }
    }

    free(line);
    fclose(f);
    return 0;
}
```

```
$ gcc -std=c11 -Wall -Wextra examples/parse_passwd.c -o parse_passwd
$ ./parse_passwd
root             uid=0      shell=/bin/bash
patrick          uid=1000   shell=/bin/bash
$ ./parse_passwd /etc/passwd    # same thing, explicit path
root             uid=0      shell=/bin/bash
patrick          uid=1000   shell=/bin/bash
```

Why this matters for the capstone: enumerating who can log in (and with what shell) is
step one of assessing a host. A UID-0 account that isn't `root`, or a service account with
a real login shell, is exactly the kind of thing you'd flag. You now have the parsing core.

### Binary I/O: `fread` / `fwrite`

For non-text data you read and write raw bytes. `fread`/`fwrite` move a count of
fixed-size records. [`examples/fwrite_struct.c`](examples/fwrite_struct.c) writes an array
of structs to a file and reads them back:

```c
#include <stdio.h>
#include <string.h>

typedef struct {
    int  id;
    char name[16];
} record_t;

int main(void) {
    record_t out[2] = { {1, "alpha"}, {2, "beta"} };

    FILE *f = fopen("/tmp/records.bin", "wb");
    if (f == NULL) { perror("open for write"); return 1; }
    size_t w = fwrite(out, sizeof(record_t), 2, f);   /* returns #records written */
    if (w != 2) { fprintf(stderr, "short write (%zu)\n", w); fclose(f); return 1; }
    fclose(f);

    record_t in[2];
    f = fopen("/tmp/records.bin", "rb");
    if (f == NULL) { perror("open for read"); return 1; }
    size_t r = fread(in, sizeof(record_t), 2, f);     /* returns #records read */
    if (r != 2) { fprintf(stderr, "short read (%zu)\n", r); fclose(f); return 1; }
    fclose(f);

    for (int i = 0; i < 2; i++) {
        printf("id=%d name=%s\n", in[i].id, in[i].name);
    }
    return 0;
}
```

```
$ gcc -std=c11 -Wall -Wextra examples/fwrite_struct.c -o fwrite_struct
$ ./fwrite_struct
id=1 name=alpha
id=2 name=beta
```

`fwrite`/`fread` return a *count of records*, not bytes — compare it to the count you asked
for. Note: a binary file written this way is **not portable** across machines with
different word sizes, struct padding, or byte order. Fine for a scratch file on one host;
use an explicit text or serialization format for data that travels.

---

## Common mistakes / gotchas

- **Not checking `fopen`/`open`.** `NULL` or `-1` used blindly is an immediate crash or
  silent corruption. Check every time.
- **Forgetting `fgets` keeps the `\n`.** Strip it before comparing or printing inline.
- **Assuming `read`/`write` move everything.** They can do less; loop until done (see
  `write_all`).
- **Reading `errno` too late.** Check the return value first; read `errno` on the very next
  line. An intervening successful call may have clobbered it.
- **Using `strtok` on `/etc/passwd`.** It merges `::` into one delimiter and misaligns
  fields. Split manually or with `strsep` (which needs a feature-test macro).
- **Leaking descriptors/streams.** Every `open` needs a `close`; every `fopen` a `fclose`.
  In long-running tools you'll hit `EMFILE` (too many open files) if you don't.
- **Mixing stream and fd I/O on the same open file** without flushing — buffers desync.
- **`getline`'s buffer.** You `free` it once after the loop, and you can start with
  `line = NULL, cap = 0` so it allocates on first use.

## Recap

- C has two file APIs: buffered **streams** (`FILE *`) and raw **file descriptors** (`int`).
  Use streams for text/formatted I/O, descriptors for control and non-file objects.
- Every I/O call can fail and signals it with a return value plus `errno`. Check the
  return, then report with `perror`/`strerror`. This is the "check every call" discipline.
- `fgets`/`getline` read lines; `getline` grows its own buffer (needs `_POSIX_C_SOURCE`).
- `read`/`write` may transfer fewer bytes than requested — loop.
- You can parse structured text (like `/etc/passwd`) by reading lines and splitting fields;
  mind empty fields and malformed input.

## Exercises

See [`exercises/README.md`](exercises/README.md).

## Further reading

- `man` pages: `fopen(3)`, `fgets(3)`, `getline(3)`, `fread(3)`, `open(2)`, `read(2)`,
  `write(2)`, `close(2)`, `errno(3)`, `perror(3)`, `strerror(3)`.
- `man 5 passwd` — the format of `/etc/passwd`.
- Kernighan & Ritchie, *The C Programming Language*, chapter 7 (standard I/O).
