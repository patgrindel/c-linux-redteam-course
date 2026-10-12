# C Programming for Linux — From Fundamentals to a Red Team Tool

A self-paced, ~4–6 week introductory course that takes you from zero C experience to
writing a real, multi-file Linux command-line tool. It is written for people who
already know some programming (Python especially) but have **never written C**.

The course ends with a **capstone**: building a small Linux host-enumeration /
recon tool of the kind you'd reach for while studying for the **OSCP**. Everything
here is for learning and for use on systems you own or are authorized to test.

---

## Who this is for

- You can write a loop, a function, and a dictionary in Python.
- You have **no** C experience (or you tried once and bounced off pointers).
- You run, or can run, Linux (a VM, WSL2, or a cloud box is fine). See [SETUP.md](SETUP.md).
- You want to understand what's *actually happening* — memory, the compiler, syscalls —
  not just copy snippets.

## What you'll be able to do by the end

- Read and write idiomatic C and reason about pointers and memory.
- Compile, link, and debug multi-file projects with `gcc`, `make`, `gdb`, and `valgrind`.
- Use the Linux system-call interface: files, processes, signals, sockets.
- Build a working CLI tool that enumerates a Linux host and reports findings.

---

## How the course is organized

Each module is a folder under [`modules/`](modules/) containing:

- `README.md` — the lesson. Read it top to bottom. Type the examples yourself.
- `examples/` — complete, compilable programs referenced by the lesson.
- `exercises/` — tasks for you to do, with a `README.md` describing each.
- `solutions/` — reference solutions. **Try the exercise first.**

Work the modules in order — each builds on the last. Budget the times as a guide,
not a rule.

| # | Module | Focus | Est. time |
|---|--------|-------|-----------|
| 00 | [Setup & the C Toolchain](modules/00-setup-and-toolchain) | Environment, gcc, your first build | 0.5 day |
| 01 | [From Python to C: The Compilation Model](modules/01-python-to-c) | How C differs, the build pipeline, `main` | 1 day |
| 02 | [Types, Variables & Operators](modules/02-types-and-variables) | Fixed-width types, integer/float behavior, I/O | 2 days |
| 03 | [Control Flow & Functions](modules/03-control-flow-and-functions) | `if`/`switch`/loops, functions, scope | 2 days |
| 04 | [Pointers & the Memory Model](modules/04-pointers-and-memory) | The core topic. Addresses, deref, `&`/`*` | 3 days |
| 05 | [Arrays & Strings](modules/05-arrays-and-strings) | C strings, the standard string library, bounds | 3 days |
| 06 | [Structs, Enums & Unions](modules/06-structs-enums-unions) | Aggregates, `typedef`, data modeling | 2 days |
| 07 | [Dynamic Memory & the Heap](modules/07-dynamic-memory) | `malloc`/`free`, ownership, leaks, lifetimes | 3 days |
| 08 | [File I/O & Error Handling](modules/08-file-io-and-errors) | Streams vs fds, `errno`, robust I/O | 2 days |
| 09 | [The Linux Syscall Interface](modules/09-linux-syscalls-and-processes) | `fork`/`exec`/`wait`, signals, `/proc` | 3 days |
| 10 | [Networking with Sockets](modules/10-sockets-and-networking) | TCP/UDP clients, a tiny port scanner | 3 days |
| 11 | [Real Projects: Make, Headers & Debugging](modules/11-build-systems-and-debugging) | Multi-file builds, `gdb`, `valgrind`, hygiene | 2 days |
| 12 | [Capstone: A Linux Recon Tool](modules/12-capstone-recon-tool) | Put it all together for OSCP-style enumeration | 1 week |

See [COURSE_OUTLINE.md](COURSE_OUTLINE.md) for the detailed syllabus.

### Companion references

Keep these open while you work:

- [CHEATSHEET.md](CHEATSHEET.md) — `gcc` / `gdb` / `valgrind` commands, format specifiers, the memory model, `malloc` discipline, key `man` pages.
- [GLOSSARY.md](GLOSSARY.md) — plain-language definitions of every term the course uses.

---

## Getting started

1. Read [SETUP.md](SETUP.md) and get a working Linux + `gcc` environment.
2. Open [`modules/00-setup-and-toolchain`](modules/00-setup-and-toolchain) and go.
3. Keep a scratch directory for experiments. Break things on purpose.

## A note on the security content

This course is for **authorized** learning: your own machines, lab VMs (HTB/THM),
and OSCP exam-style targets you are explicitly permitted to test. The capstone is a
*defensive/assessment* tool — it inspects a host you control and reports its posture.
Nothing here is for use against systems you don't own or lack written permission to test.

---

## License

MIT — see [LICENSE](LICENSE). Educational use encouraged.
