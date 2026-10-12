# Detailed Course Outline

Each module lists its learning objectives and the key topics covered. Modules build
on each other; do them in order.

---

## Module 00 — Setup & the C Toolchain
**Objective:** Have a working environment and understand what a compiler does.
- The pieces: preprocessor, compiler, assembler, linker
- `gcc hello.c -o hello`, running it, exit codes
- `-Wall -Wextra` and why warnings are not optional
- Reading errors vs. warnings

## Module 01 — From Python to C: The Compilation Model
**Objective:** Understand the mental shift from interpreted to compiled, statically typed C.
- Compiled vs. interpreted; what the binary *is*
- The `main` function and `return` as the exit code
- `#include`, headers, and declarations vs. definitions
- Static typing, manual memory, no garbage collector — what that means for you
- A side-by-side Python-vs-C tour

## Module 02 — Types, Variables & Operators
**Objective:** Use C's type system correctly and predict integer/float behavior.
- `int`, `char`, `short`, `long`, `float`, `double`, `_Bool`
- Fixed-width types `<stdint.h>` (`uint8_t`, `int32_t`, …) and when to use them
- Signed vs. unsigned, overflow, integer promotion, truncation
- Operators, precedence, bitwise ops (critical for later)
- `printf`/`scanf` format specifiers; reading input safely

## Module 03 — Control Flow & Functions
**Objective:** Structure programs with functions, understand scope and the stack.
- `if`/`else`, `switch`, `while`, `do/while`, `for`, `break`/`continue`
- Functions: declaration, definition, parameters **by value**
- Scope, lifetime, `static` and `extern`, the call stack
- Multiple return values via pointers (preview of pointers)

## Module 04 — Pointers & the Memory Model
**Objective:** The central skill. Reason confidently about addresses and memory.
- What a pointer *is*; `&` (address-of) and `*` (dereference)
- The stack vs. the heap vs. static storage; memory layout of a process
- Pointer arithmetic, `NULL`, null-deref crashes
- Pointers to functions (brief), `const` correctness
- Passing by reference; "output parameters"

## Module 05 — Arrays & Strings
**Objective:** Work with C's array/string model without buffer overflows.
- Arrays, array-pointer decay, indexing as arithmetic
- C strings are NUL-terminated `char` arrays
- `<string.h>`: `strlen`, `strcpy`/`strncpy`, `strcmp`, `memcpy`, `strtok`, …
- The classic bugs: off-by-one, missing NUL, buffer overflow — and safe patterns
- `argv`/`argc` and parsing command-line arguments

## Module 06 — Structs, Enums & Unions
**Objective:** Model structured data.
- `struct`, member access `.` and `->`, `typedef`
- Nested structs, arrays of structs
- `enum` for named constants; `union` and when it's useful
- Memory layout, padding/alignment (awareness level)

## Module 07 — Dynamic Memory & the Heap
**Objective:** Manage memory manually and safely.
- `malloc`, `calloc`, `realloc`, `free`; checking for `NULL`
- Ownership, lifetimes, and who frees what
- Leaks, use-after-free, double-free — and how `valgrind` finds them
- Building a dynamic array / growable buffer

## Module 08 — File I/O & Error Handling
**Objective:** Read/write files robustly and handle errors the C way.
- Streams (`FILE*`: `fopen`/`fread`/`fgets`/`fprintf`/`fclose`)
- File descriptors (`open`/`read`/`write`/`close`) and how they differ
- `errno`, `perror`, `strerror`; the "check every call" discipline
- Reading a file line by line; parsing structured text (e.g. `/etc/passwd`)

## Module 09 — The Linux Syscall Interface
**Objective:** Use the OS directly: processes, signals, and `/proc`.
- What a system call is; `strace` to watch them
- `fork`, `exec*`, `wait`/`waitpid`; running an external command
- Pipes and capturing a child's output
- Signals basics (`signal`/`sigaction`, `SIGINT`, `SIGCHLD`)
- Reading `/proc` for process and system info

## Module 10 — Networking with Sockets
**Objective:** Write TCP/UDP clients and understand the socket API.
- The Berkeley sockets API: `socket`/`connect`/`send`/`recv`/`close`
- Addresses: `struct sockaddr_in`, `getaddrinfo`, byte order (`htons`)
- A TCP connect client; grabbing a service banner
- Non-blocking connects and timeouts; a minimal concurrent port scanner

## Module 11 — Real Projects: Make, Headers & Debugging
**Objective:** Organize, build, and debug a multi-file project.
- Splitting code into `.c`/`.h`; include guards; the one-definition rule
- Writing a `Makefile`; incremental builds; `CFLAGS`
- `gdb`: breakpoints, stepping, inspecting memory, backtraces on crashes
- `valgrind` and sanitizers (`-fsanitize=address,undefined`)
- Project hygiene: structure, error handling, documentation

## Module 12 — Capstone: A Linux Recon Tool
**Objective:** Build a real, multi-file CLI tool that enumerates a Linux host and
reports its security posture — the kind of local enumeration you do early in an
OSCP-style engagement on a box you're authorized to assess.
- Spec & architecture; argument parsing; output formatting
- Checks: OS/kernel info, users & groups from `/etc/passwd`, SUID/SGID binaries,
  world-writable files, interesting cron entries, listening sockets, env/PATH issues
- Pulling together: file I/O (08), `/proc` and process spawning (09), sockets (10),
  dynamic data structures (07), clean multi-file build (11)
- Stretch goals, a reflection, and where to go next (the Linux Programming Interface,
  secure-coding practices, contributing to real tools)

Everything in the capstone inspects the **local** host you run it on, or targets you
are explicitly authorized to test. It is an assessment aid, not an attack tool.
