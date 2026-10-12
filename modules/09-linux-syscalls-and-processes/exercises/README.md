# Module 09 — Exercises

Build with `gcc -std=c11 -Wall -Wextra` and the `#define _POSIX_C_SOURCE 200809L` shown in
the examples (POSIX functions like `fork`, `waitpid`, `sigaction` need it under `-std=c11`).
Check every call. Keep signal handlers tiny.

Reference solutions in [`solutions/`](../solutions). All are local, authorized-host tasks.

---

## 1. `mysystem` — run a command string

**Goal:** Write `int mysystem(const char *cmd)` that runs `cmd` via `/bin/sh -c` (so shell
syntax works), waits for it, and returns its exit status (or `-1` on failure). Demonstrate
it from `main` by running a command passed as `argv[1]`.

**Constraints:**
- `fork`, then in the child `execl("/bin/sh", "sh", "-c", cmd, (char *)NULL)`.
- On a failed `exec`, `perror` and `_exit(127)`.
- Return `WEXITSTATUS` when the child exited normally.

**Verify:**
```
$ ./ex1 'echo hi && exit 3' ; echo "status=$?"
hi
status=3
```

Starter: [`ex1_starter.c`](ex1_starter.c).

---

## 2. Capture and summarize command output

**Goal:** Run `uname -a` (hardcoded) with the pipe-capture pattern from
`examples/run_capture.c`, then print the captured string **and** its length in bytes.

**Constraints:**
- Use `pipe` + `fork` + `dup2` + `execvp`.
- Parent must close the write end before reading.
- Free the captured buffer before exit.

**Verify:**
```
$ ./ex2
Linux mybox 6.8.0-45-generic #45-Ubuntu SMP ... x86_64 GNU/Linux
captured 95 bytes
```

---

## 3. Two-strike Ctrl-C

**Goal:** A program that loops forever, but prints `press Ctrl-C again to quit` on the
first `SIGINT` and only exits on the **second** one within the run.

**Constraints:**
- Use `sigaction`. The handler may only touch a `volatile sig_atomic_t` counter (and,
  optionally, `write` a fixed string — no `printf` in the handler).
- `main` decides when to exit based on the counter.

**Verify:** First Ctrl-C prints the warning and keeps running; second Ctrl-C exits 0.

---

## 4. `self status` summary

**Goal:** Print this process's `Name`, `Pid`, `PPid`, and `VmRSS` by reading
`/proc/self/status`, formatted as `key = value` (value trimmed of leading whitespace).

**Constraints:**
- Read line by line; match the keys with `strncmp`.
- Trim the tab/spaces after the `:` so the value prints cleanly.

**Verify:**
```
$ ./ex4
Name = ex4
Pid = 7123
PPid = 3990
VmRSS = 1540 kB
```

---

## 5. List running processes

**Goal:** Scan `/proc` for numeric subdirectories (each is a PID) and print, for each, the
PID and the process name from `/proc/<pid>/comm`, sorted or unsorted.

**Constraints:**
- Use `opendir`/`readdir` (from `<dirent.h>`). A directory entry is a PID if its name is
  all digits.
- Skip entries that disappear between listing and reading (a process can exit mid-scan) —
  just continue on `fopen` failure.
- `/proc/<pid>/comm` is a single line with the command name.

**Verify:**
```
$ ./ex5 | head -3
    1 systemd
    2 kthreadd
  ...
$ ./ex5 | wc -l      # roughly matches `ps -e | wc -l`
```

This is the process-enumeration building block for the capstone. Starter:
[`ex5_starter.c`](ex5_starter.c).
