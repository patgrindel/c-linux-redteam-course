# Module 09 — The Linux Syscall Interface: Processes, Signals & `/proc`

Talking to the kernel directly: watching system calls with `strace`, spawning and reaping
child processes, capturing a command's output through a pipe, handling signals, and reading
`/proc` for live system facts.

## Where we are

Module 08 showed that `read`/`write`/`open` are thin wrappers over **system calls** — the
boundary where your program asks the kernel to do something it can't do itself. This module
is about that boundary: how to create processes, run other programs, and read the kernel's
own view of the machine. This is the heart of "C for Linux".

## Learning objectives

By the end you can:

- Explain what a system call is and watch a program's syscalls with `strace`.
- Create processes with `fork(2)`, replace a process image with the `exec*` family, and
  collect exit status with `wait(2)`/`waitpid(2)`.
- Run an external command and capture its output through a `pipe(2)`.
- Install signal handlers with `sigaction(2)`, handle `SIGINT`, and reap children on
  `SIGCHLD`.
- Read `/proc` for process and system information.

Everything here is framed for **authorized host assessment** — enumerating a box you own
or are permitted to test. Spawning tools, reading `/proc`, and listing processes are the
mechanics behind the capstone's local-recon checks.

---

## What is a system call?

Your process runs in **user mode** and cannot directly touch hardware, other processes, or
kernel data. When it needs the OS to act — open a file, create a process, send bytes on a
socket — it makes a **system call**: a controlled switch into **kernel mode** that runs
kernel code on your behalf, then returns.

Library functions like `printf` are *not* syscalls; they do formatting in userspace and
eventually call the `write(2)` syscall. `malloc` eventually calls `mmap(2)`/`brk(2)`.
Knowing which is which tells you where to look when something is slow or failing.

### Watching syscalls with `strace`

`strace` runs a program and prints every syscall it makes, with arguments and return
values. It is the fastest way to understand what a program is *actually* doing — and, on a
host you're assessing, to see what a suspicious binary touches.

Take this trivial program, [`examples/hello_write.c`](examples/hello_write.c):

```c
#include <unistd.h>
#include <string.h>

int main(void) {
    const char *msg = "hi\n";
    write(STDOUT_FILENO, msg, strlen(msg));   /* one syscall, no stdio buffering */
    return 0;
}
```

```
$ gcc -std=c11 -Wall -Wextra examples/hello_write.c -o hello_write
$ strace -f ./hello_write
execve("./hello_write", ["./hello_write"], 0x7ffd... /* 60 vars */) = 0
brk(NULL)                               = 0x55e8...
...                                     (dynamic-linker and libc setup)
write(1, "hi\n", 3)                     = 3
exit_group(0)                           = ?
+++ exited with 0 +++
hi
```

Read that `write(1, "hi\n", 3) = 3` line as: syscall `write`, to fd `1` (stdout), buffer
`"hi\n"`, length `3`, returned `3` (bytes written). Useful `strace` flags: `-f` follow
forked children, `-e trace=open,openat,read,write` filter, `-o out.txt` write to a file,
`-p <pid>` attach to a running process (needs permission).

---

## Creating processes: `fork(2)`

`fork` is the Unix way to make a new process. It **clones** the caller: the child gets a
copy of the parent's memory, open files, everything. It returns **twice** — once in each
process:

- In the **parent**: the child's PID (a positive number).
- In the **child**: `0`.
- On failure: `-1` in the parent, no child created.

There is no Python analogue this direct; think of it as "the program splits in two here,
and the only difference between the halves is this return value."

[`examples/fork_basic.c`](examples/fork_basic.c):

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    printf("before fork: pid=%d\n", (int) getpid());

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* child */
        printf("  child:  my pid=%d, parent=%d\n", (int) getpid(), (int) getppid());
        return 42;                  /* becomes the child's exit status */
    }

    /* parent */
    printf("parent: forked child pid=%d\n", (int) pid);

    int status;
    if (waitpid(pid, &status, 0) < 0) {   /* block until this child ends */
        perror("waitpid");
        return 1;
    }
    if (WIFEXITED(status)) {
        printf("parent: child exited with %d\n", WEXITSTATUS(status));
    }
    return 0;
}
```

```
$ gcc -std=c11 -Wall -Wextra examples/fork_basic.c -o fork_basic
$ ./fork_basic
before fork: pid=4120
parent: forked child pid=4121
  child:  my pid=4121, parent=4120
parent: child exited with 42
```

(PIDs differ each run; parent/child print order can vary — they run concurrently.)

**Why `waitpid`?** When a child exits, the kernel keeps a little status record until the
parent collects it. An uncollected dead child is a **zombie**. `wait`/`waitpid` collects
it ("reaps" it) and gives you the exit status. The macros:

- `WIFEXITED(status)` — true if it exited normally; `WEXITSTATUS(status)` is its return code.
- `WIFSIGNALED(status)` — true if a signal killed it; `WTERMSIG(status)` is the signal.

---

## Running another program: `exec*` + `fork` + `wait`

`fork` alone just duplicates your program. To run a *different* program, the child calls
one of the `exec*` functions, which **replace** the current process image with a new one.
On success `exec` never returns (the old code is gone); on failure it returns `-1`.

The family differs in how you pass arguments and whether `PATH` is searched:

| Function | Args as | Searches `PATH`? |
|----------|---------|------------------|
| `execv`  | array   | no (needs full path) |
| `execvp` | array   | yes |
| `execl`  | list    | no |
| `execlp` | list    | yes |

The common pattern — **fork, then exec in the child, then wait in the parent** — is exactly
what a shell does for every command. [`examples/fork_exec_wait.c`](examples/fork_exec_wait.c)
runs a command given on its own command line:

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <cmd> [args...]\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* child: argv+1 is already a NULL-terminated array of strings */
        execvp(argv[1], &argv[1]);
        perror("execvp");       /* only reached if exec failed */
        _exit(127);             /* 127 = "command not found", like a shell */
    }

    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return 1;
    }
    if (WIFEXITED(status)) {
        printf("[exited %d]\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("[killed by signal %d]\n", WTERMSIG(status));
    }
    return 0;
}
```

```
$ gcc -std=c11 -Wall -Wextra examples/fork_exec_wait.c -o fork_exec_wait
$ ./fork_exec_wait uname -sr
Linux 6.8.0-45-generic
[exited 0]
$ ./fork_exec_wait nosuchcmd
execvp: No such file or directory
[exited 127]
```

Note `_exit(127)` (not `return`/`exit`) in the child after a failed `exec`: the child
shares the parent's flushed-or-not stdio buffers, and `_exit` avoids double-flushing them.
`argv` is already `NULL`-terminated by the C runtime, which is why `&argv[1]` works as the
exec argument vector.

---

## Capturing a command's output with `pipe(2)`

Often you don't just want to run a command — you want its **output** as a string (to parse
it, as the capstone does with system tools). A `pipe` is a one-way in-kernel channel: it
gives you two fds, `fd[0]` to read and `fd[1]` to write. Point the child's stdout at the
write end with `dup2(2)`, run the command, and read the read end in the parent.

[`examples/run_capture.c`](examples/run_capture.c):

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* Run argv-style command, capture its stdout into *out (malloc'd, NUL-terminated).
   Returns the child's exit status, or -1 on setup failure. Caller frees *out. */
static int run_capture(char *const cmd[], char **out) {
    int fds[2];
    if (pipe(fds) < 0) {
        perror("pipe");
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        close(fds[0]);
        close(fds[1]);
        return -1;
    }

    if (pid == 0) {
        /* child: send stdout into the pipe, then exec */
        close(fds[0]);                  /* child doesn't read */
        if (dup2(fds[1], STDOUT_FILENO) < 0) {
            perror("dup2");
            _exit(127);
        }
        close(fds[1]);
        execvp(cmd[0], cmd);
        perror("execvp");
        _exit(127);
    }

    /* parent: read everything from the pipe */
    close(fds[1]);                      /* parent doesn't write; lets reader see EOF */

    size_t cap = 4096, len = 0;
    char *buf = malloc(cap);
    if (buf == NULL) {
        perror("malloc");
        close(fds[0]);
        return -1;
    }

    ssize_t n;
    while ((n = read(fds[0], buf + len, cap - len - 1)) > 0) {
        len += (size_t) n;
        if (len + 1 >= cap) {           /* grow, keep room for the NUL */
            cap *= 2;
            char *bigger = realloc(buf, cap);
            if (bigger == NULL) {
                perror("realloc");
                free(buf);
                close(fds[0]);
                return -1;
            }
            buf = bigger;
        }
    }
    buf[len] = '\0';
    close(fds[0]);

    int status;
    waitpid(pid, &status, 0);
    *out = buf;
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}

int main(void) {
    char *const cmd[] = { "id", "-un", NULL };   /* print the current username */
    char *output = NULL;

    int rc = run_capture(cmd, &output);
    if (rc < 0) {
        return 1;
    }
    printf("exit=%d, captured: %s", rc, output);   /* output already ends in '\n' */
    free(output);
    return 0;
}
```

```
$ gcc -std=c11 -Wall -Wextra examples/run_capture.c -o run_capture
$ ./run_capture
exit=0, captured: patrick
```

The ordering is load-bearing: the parent must `close(fds[1])` (its copy of the write end)
or the final `read` never sees EOF — the pipe stays "open for writing" as long as *any*
process holds the write end. This capture-via-pipe pattern is how your capstone will shell
out to tools like `uname` or `getcap` and parse the result.

---

## Signals

A **signal** is an asynchronous notification to a process: `SIGINT` (Ctrl-C), `SIGTERM`
(polite kill), `SIGKILL` (unstoppable), `SIGSEGV` (you dereferenced bad memory), `SIGCHLD`
(a child changed state). You can install a **handler** to run when one arrives.

Prefer `sigaction(2)` over the older `signal(2)` — its behavior is portable and well
defined. Handlers run at arbitrary times, so they must only do **async-signal-safe** work:
set a flag, or call a short list of safe syscalls like `write`. Don't call `printf` or
`malloc` from a handler.

[`examples/signal_sigint.c`](examples/signal_sigint.c) — catch Ctrl-C, finish cleanly:

```c
#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

static volatile sig_atomic_t stop = 0;   /* the only safe type to touch in a handler */

static void on_sigint(int sig) {
    (void) sig;
    stop = 1;                             /* just flag it; act in main() */
}

int main(void) {
    struct sigaction sa = {0};
    sa.sa_handler = on_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;                      /* no SA_RESTART: let blocking calls return EINTR */
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("sigaction");
        return 1;
    }

    printf("working... press Ctrl-C to stop\n");
    unsigned long i = 0;
    while (!stop) {
        i++;
        if (i % 100000000UL == 0) {
            printf("still working (%lu)\n", i);
            fflush(stdout);
        }
    }

    printf("\ncaught SIGINT, cleaning up, bye\n");
    return 0;
}
```

```
$ gcc -std=c11 -Wall -Wextra examples/signal_sigint.c -o signal_sigint
$ ./signal_sigint
working... press Ctrl-C to stop
still working (100000000)
^C
caught SIGINT, cleaning up, bye
```

`volatile sig_atomic_t` is the one type the standard guarantees you can read/write safely
between a handler and `main`. The flag pattern — handler sets, main loop checks — is the
clean way to get an interruptible program.

### Reaping children on `SIGCHLD`

When children can finish at any time (e.g. a server spawning workers), reap them from a
`SIGCHLD` handler using `waitpid(-1, ..., WNOHANG)` in a loop, so no zombies accumulate.
[`examples/sigchld_reap.c`](examples/sigchld_reap.c):

```c
#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static void reap(int sig) {
    (void) sig;
    int saved = errno_placeholder();      /* see note below */
    (void) saved;
    /* Reap every child that has finished, without blocking. */
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        /* loop */
    }
}

int main(void) {
    struct sigaction sa = {0};
    sa.sa_handler = reap;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;   /* restart interrupted calls; only on exit */
    if (sigaction(SIGCHLD, &sa, NULL) < 0) {
        perror("sigaction");
        return 1;
    }

    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            printf("  child %d (pid %d) running\n", i, (int) getpid());
            _exit(0);
        }
    }

    /* Parent does its own work; the handler reaps children as they exit. */
    for (int t = 0; t < 5; t++) {
        printf("parent tick %d\n", t);
        sleep(1);
    }
    printf("parent done, no zombies left behind\n");
    return 0;
}
```

> Note: the `errno_placeholder()` call above is pseudo-code to flag a real rule — a handler
> should **save and restore `errno`** (`int saved = errno; ... ; errno = saved;`) because
> `waitpid` can change it and you don't want to corrupt `errno` for the interrupted main
> code. The compilable version in `examples/sigchld_reap.c` does exactly that with
> `<errno.h>`. Build and run it:

```
$ gcc -std=c11 -Wall -Wextra examples/sigchld_reap.c -o sigchld_reap
$ ./sigchld_reap
parent tick 0
  child 0 (pid 5001) running
  child 1 (pid 5002) running
  child 2 (pid 5003) running
parent tick 1
parent tick 2
parent tick 3
parent tick 4
parent done, no zombies left behind
```

(Child lines interleave with the parent's ticks; exact order varies.)

---

## Reading `/proc`

Linux exposes kernel and process state as **files** under `/proc`. No special API — you
just open and read text (Module 08 skills apply directly). This is the richest source of
host facts for assessment, and it's all world-readable where policy allows.

- `/proc/version` — kernel version string.
- `/proc/cpuinfo`, `/proc/meminfo` — hardware/memory.
- `/proc/<pid>/status` — per-process: name, PIDs, UIDs/GIDs, memory.
- `/proc/<pid>/cmdline` — the command line (NUL-separated).
- `/proc/self/` — a symlink to the calling process's own `/proc/<pid>`.
- Numeric subdirectories of `/proc` are exactly the running PIDs.

[`examples/proc_version.c`](examples/proc_version.c) reads one line:

```c
#include <stdio.h>

int main(void) {
    FILE *f = fopen("/proc/version", "r");
    if (f == NULL) {
        perror("/proc/version");
        return 1;
    }
    char line[512];
    if (fgets(line, sizeof line, f) != NULL) {
        fputs(line, stdout);
    }
    fclose(f);
    return 0;
}
```

```
$ gcc -std=c11 -Wall -Wextra examples/proc_version.c -o proc_version
$ ./proc_version
Linux version 6.8.0-45-generic (buildd@lcy02...) (gcc ...) #45-Ubuntu SMP ...
```

[`examples/proc_status.c`](examples/proc_status.c) parses `key:\tvalue` lines from a
process's status file (defaults to `self`):

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    char path[64];
    if (argc == 2) {
        snprintf(path, sizeof path, "/proc/%s/status", argv[1]);
    } else {
        snprintf(path, sizeof path, "/proc/self/status");
    }

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        perror(path);
        return 1;
    }

    /* Pull out a few interesting fields. Lines look like "Name:\tbash". */
    char line[256];
    while (fgets(line, sizeof line, f) != NULL) {
        if (strncmp(line, "Name:", 5) == 0 ||
            strncmp(line, "Pid:", 4) == 0 ||
            strncmp(line, "PPid:", 5) == 0 ||
            strncmp(line, "Uid:", 4) == 0 ||
            strncmp(line, "VmRSS:", 6) == 0) {
            fputs(line, stdout);
        }
    }
    fclose(f);
    return 0;
}
```

```
$ gcc -std=c11 -Wall -Wextra examples/proc_status.c -o proc_status
$ ./proc_status            # inspect ourselves
Name:	proc_status
Pid:	6042
PPid:	3990
Uid:	1000	1000	1000	1000
VmRSS:	   1536 kB
$ ./proc_status 1          # inspect PID 1 (init/systemd)
Name:	systemd
Pid:	1
PPid:	0
Uid:	0	0	0	0
VmRSS:	  11240 kB
```

The `Uid:` line shows real/effective/saved/filesystem UIDs. A process running as effective
UID 0 that you didn't expect, or one owned by a service account with a surprising command
line, is exactly what you'd note when assessing a host.

---

## Common mistakes / gotchas

- **Forgetting to reap children.** Dead, uncollected children become zombies. `wait`/
  `waitpid`, or a `SIGCHLD` handler.
- **Assuming `exec` returns.** On success it doesn't. Code after it runs *only* on failure —
  report and `_exit`.
- **Using `return`/`exit` in a child after a failed `exec`.** Use `_exit` to avoid
  double-flushing inherited stdio buffers.
- **Not closing the pipe's write end in the parent.** The reader then never sees EOF and
  `read` blocks forever.
- **Doing unsafe work in a signal handler.** No `printf`/`malloc`. Set a
  `volatile sig_atomic_t` flag, or use only async-signal-safe calls; save/restore `errno`.
- **Ignoring `EINTR`.** A blocking syscall interrupted by a signal returns `-1` with
  `errno == EINTR`. Decide: retry the call, or let it unwind (`SA_RESTART` auto-retries).
- **Reading `/proc` as binary.** It's text; `/proc/.../cmdline` uses NUL separators, so
  replace NULs with spaces when printing.

## Recap

- A **system call** is the controlled entry into the kernel; `strace` shows them live.
- `fork` clones a process (returns twice); `exec*` replaces the image; `wait`/`waitpid`
  collects exit status and prevents zombies.
- A `pipe` plus `dup2` lets a parent capture a child's output — the basis for shelling out
  and parsing results.
- Handle signals with `sigaction`; keep handlers tiny (flag + async-safe calls) and reap
  children on `SIGCHLD`.
- `/proc` exposes kernel and per-process state as plain text files you read with ordinary
  file I/O.

## Exercises

See [`exercises/README.md`](exercises/README.md).

## Further reading

- `man` pages: `fork(2)`, `execve(2)`, `execvp(3)`, `wait(2)`, `waitpid(2)`, `pipe(2)`,
  `dup2(2)`, `signal(7)`, `sigaction(2)`, `strace(1)`, `proc(5)`.
- Michael Kerrisk, *The Linux Programming Interface*, chapters 24–27 (processes), 20–22
  (signals).
