# C / gcc / gdb Quick Reference

A one-page companion. Skim it now; it'll make more sense as you work the modules.

## Compiling with gcc

```bash
gcc -std=c11 -Wall -Wextra -o prog prog.c   # compile one file, warnings on
gcc -std=c11 -Wall -Wextra -g -o prog prog.c # add -g for debug symbols (gdb)
gcc -std=c11 -Wall -Wextra -O2 -o prog prog.c # optimize (release)
gcc -std=gnu11 ...                            # enable GNU/POSIX extensions
./prog ; echo "exit code: $?"                 # run, then show exit status
```

Multi-file:

```bash
gcc -std=c11 -Wall -Wextra -c a.c   # -> a.o   (compile only, no link)
gcc -std=c11 -Wall -Wextra -c b.c   # -> b.o
gcc -o prog a.o b.o                 # link into an executable
```

Useful flags: `-g` (debug info), `-O0/-O2` (optimization), `-I dir` (header search),
`-D NAME=val` (define a macro), `-fsanitize=address,undefined` (catch memory/UB bugs).

## Essential format specifiers (printf/scanf)

| Type | Specifier | Notes |
|------|-----------|-------|
| `int` | `%d` | |
| `unsigned int` | `%u` | |
| `long` / `unsigned long` | `%ld` / `%lu` | |
| `size_t` | `%zu` | the right one for `sizeof`/lengths |
| `float`/`double` | `%f` | `double` is the default for `%f` |
| `char` | `%c` | |
| string (`char*`) | `%s` | must be NUL-terminated |
| pointer | `%p` | cast the arg: `(void*)p` |
| hex | `%x` / `%X` | |
| `int32_t` etc. | `PRId32` | from `<inttypes.h>`: `printf("%" PRId32, v)` |

## Memory: the four regions

```
text     machine code (read-only)
data/bss global & static variables
heap     malloc/free — you manage lifetime   (grows up)
stack    locals & call frames — automatic     (grows down)
```

## malloc discipline

```c
int *p = malloc(n * sizeof *p);   // size from the pointee, not a hard-coded type
if (p == NULL) { /* handle out-of-memory */ }
/* ... use p ... */
free(p);
p = NULL;                         // avoid dangling use-after-free
```

Rules: every `malloc` has exactly one `free`; don't use after `free`; don't `free` twice;
`free(NULL)` is safe and does nothing.

## gdb survival kit

```bash
gcc -g -O0 -o prog prog.c
gdb ./prog
```

```
(gdb) break main          # or: break file.c:42
(gdb) run [args]          # start
(gdb) next   / n          # step over
(gdb) step   / s          # step into
(gdb) continue / c        # resume
(gdb) print expr / p      # inspect a variable/expression
(gdb) backtrace / bt      # call stack (use right after a crash!)
(gdb) info locals         # all locals in frame
(gdb) x/8xw &arr          # examine memory: 8 hex words at &arr
(gdb) quit
```

After a segfault, just run under gdb and type `bt` — it tells you the exact line.

## valgrind (memory errors & leaks)

```bash
gcc -g -O0 -o prog prog.c
valgrind --leak-check=full --show-leak-kinds=all ./prog
```

"Invalid read/write" = out-of-bounds or use-after-free. "definitely lost" = a leak.

## Common man pages you'll live in

```
man 3 printf     man 3 malloc     man 3 strtol
man 2 open       man 2 read       man 2 write     man 2 stat
man 2 fork       man 2 execve     man 2 waitpid
man 2 socket     man 3 getaddrinfo
man 7 signal
```

Section 2 = syscalls, section 3 = libc functions. `man man` explains the sections.
