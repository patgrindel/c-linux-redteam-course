# Module 12 — Capstone: A Linux Recon Tool

**Build `reconc`: a real, multi-file C command-line tool that enumerates the local
Linux host and reports its security posture — the local recon you do early in an
authorized, OSCP-style engagement.**

## Where we are

Modules 00–11 took you from "never written C" to building, debugging, and reasoning
about multi-file C programs. This capstone spends them all at once: file I/O and text
parsing (M08), `/proc` and process/OS facts (M09), sockets (M10), dynamic data
structures (M07), and a clean `make`/`gdb`/`valgrind` workflow (M11).

## Learning objectives

By the end you can:

- Take a vague goal ("enumerate this host") and turn it into a concrete spec and a
  module layout.
- Design a small CLI: argument parsing, a check registry, and a consistent output format.
- Implement independent "check" modules that read `/etc/passwd`, `/proc`, the
  filesystem, and sockets — each degrading gracefully when something is unreadable.
- Reuse a dynamic string/vector from Module 07 as a tiny support library.
- Build and verify the whole thing with a real `Makefile`, `valgrind`, and sanitizers.
- Judge *which findings matter to an assessor* and explain *why*.

---

> ## ⚠️ Authorized use only — read this first
>
> `reconc` is a **defensive/assessment aid**. It inspects the host it runs on and
> **reports** what it finds. It does **not** exploit anything, does **not** escalate
> privileges, does **not** try to evade detection, and does **not** attack other
> machines. Its only network activity is a connect probe to `127.0.0.1` — your own
> loopback.
>
> Run it **only** on a system you **own** or have **explicit written permission** to
> assess (your lab VM, an HTB/THM box, an OSCP-style target you're authorized against).
> Every check below is framed as *"why would an assessor note this?"* — that framing is
> the point of the whole module. The tool, its `--help`, and this README all carry this
> notice. Keep it that way.

---

## (a) Spec & goals

Before writing code, decide exactly what the tool does. Ours:

**Goal.** On the local host, collect a set of facts that an assessor checks first, and
print them grouped by topic with a severity tag.

**Checks (v1):**

| Name            | What it reports                                              |
|-----------------|-------------------------------------------------------------|
| `system`        | OS, kernel release, distro                                  |
| `users`         | UID 0 accounts and accounts with real login shells          |
| `suid`          | SUID/SGID binaries in the usual bin directories             |
| `worldwritable` | world-writable files/dirs in a few sensitive paths          |
| `cron`          | cron jobs, and any cron file writable by non-root           |
| `net`           | listening TCP sockets, and which are exposed off-box        |
| `env`           | risky `PATH` entries and loader variables                   |

**Non-goals (explicitly out of scope):** no exploitation, no privilege escalation, no
writing to the system, no scanning other hosts, no persistence. If a feature would make
this an *attack* tool rather than an *assessment* tool, it does not belong here.

**Quality bar:**

- Compiles clean under `gcc -std=gnu11 -Wall -Wextra`.
- A missing file or a permission-denied degrades to a *note*, never a crash.
- No leaks (`valgrind` clean; ASan clean).
- Output is readable by a human and greppable by a script.

Writing the spec down first is not ceremony — it's what lets you say "no" to scope creep
later, and it's exactly what a good engagement writeup starts from.

## (b) Architecture & file layout

The shape of the program follows the spec. Each check is **independent**, so each gets
its own translation unit (`.c` + `.h`). A tiny registry in `main.c` ties them together,
which means adding a check later is a three-line change, not a refactor.

```
solution/
├── Makefile
└── src/
    ├── main.c                 entry point + the check registry
    ├── args.c / args.h        command-line parsing
    ├── report.c / report.h    output helpers + strbuf/strvec (reused from M07)
    ├── util.c / util.h        small shared helpers (rstrip, basename, mode string)
    ├── version.h              one place for the name/version
    ├── checks_system.c/.h
    ├── checks_users.c/.h
    ├── checks_suid.c/.h
    ├── checks_worldwritable.c/.h
    ├── checks_cron.c/.h
    ├── checks_net.c/.h
    └── checks_env.c/.h
```

The dependency direction is simple and one-way:

```
main.c ──> checks_*.{c,h} ──> report.{c,h}, util.{c,h}
   └────────────────────────> args.{c,h}, version.h
```

Checks never call each other. They only call `report_*` to emit output and `util_*` for
shared helpers. That isolation is what makes `--only=suid` trivial and what will make the
exercises (JSON output, a `--only` filter you extend) easy.

**The one shared abstraction: the report module.** Everything printed goes through
`report.c`. Centralising output means we can add colour, severity counters, or a future
JSON mode without touching a single check. It also carries the M07 containers:

```c
/* report.h — the growable buffer and string vector from Module 07 */
typedef struct { char *data; size_t len, cap; } strbuf_t;   /* dynamic string */
typedef struct { char **items; size_t len, cap; } strvec_t; /* array of owned strings */
```

`strvec_t` is used right away (the users check collects names, sorts them, prints them).
`strbuf_t` is the buffer you'll lean on for the JSON-output stretch goal.

## (c) Argument parsing & output format design

### The CLI

Keep the surface tiny and predictable:

```
reconc [options]
  -h, --help          usage (includes the authorized-use notice)
  -V, --version
      --list          list checks and exit
      --only=LIST     run only these checks, comma-separated
  -o, --output FILE   write to FILE instead of stdout
  -v, --verbose       show skipped/unreadable scans too
      --no-color
```

We parse by hand (`args.c`) rather than pulling in `getopt_long`, so the code stays
self-contained and readable against what Module 05 taught about `argv`. The parser fills
an `options_t` with defaults first, so the rest of the program never sees an
uninitialised field:

```c
typedef struct {
    int         show_help, show_version, list_checks, no_color, verbose;
    const char *only;      /* NULL => run all */
    const char *outfile;   /* NULL => stdout */
} options_t;
```

### The output format

Design the format *before* writing checks, so every check looks the same. Three pieces:

- **Sections** — a bold banner per check: `== Users & accounts ==`.
- **Findings** — one per line, tagged by severity:
  - `[*]` **info** — a neutral fact.
  - `[+]` **note** — worth a second look.
  - `[!]` **warning** — a common posture weakness.
- **Details** — an indented, dimmed "why this matters to an assessor" line.

Severity is framed for the *reader of the report*, not for the machine. That framing —
turning "here is a SUID binary" into "here is a SUID binary, and here's why you'd flag
it" — is the difference between a data dump and an assessment aid.

Colour is emitted only when writing to a real terminal (`isatty`) and not disabled, so a
redirected file or a pipe stays plain text and greppable.

Here is the whole output API — small on purpose:

```c
void report_section(report_t *rep, const char *title);
void report_kv(report_t *rep, const char *key, const char *value);
void report_item(report_t *rep, severity_t sev, const char *fmt, ...);  /* a finding  */
void report_detail(report_t *rep, const char *fmt, ...);                /* "why" line */
void report_skip(report_t *rep, const char *fmt, ...);  /* degraded scan, verbose-only */
void report_summary(report_t *rep);                     /* final tally               */
```

`report_item` counts each finding by severity; `report_summary` prints the tally and
`main` turns "any warning?" into the process exit code.

> **Build the skeleton first.** Get `report.c`, `args.c`, and a `main` that prints the
> banner and an empty summary compiling and running *before* you write any check. A
> program that builds and does nothing beats half a check that doesn't link.

## (d) Implement each check as its own module

Every check has the same one-function interface:

```c
void check_<name>(report_t *rep);
```

It reads something, calls `report_*`, and returns. No check exits the process, and none
crashes on missing input — an assessment tool that dies on the first unreadable file is
useless. Let's build three in increasing difficulty; the rest follow the same shape and
live in `solution/src/`.

### `checks_system` — read facts (easy)

Two techniques from M09: the `uname(2)` syscall for kernel identity, and reading
`/proc/version` / `/etc/os-release` as text (M08).

```c
#define _GNU_SOURCE
#include <sys/utsname.h>

struct utsname u;
if (uname(&u) == 0) {
    report_kv(rep, "Kernel", u.release);     /* e.g. 5.15.0-91-generic */
    report_kv(rep, "Arch",   u.machine);     /* e.g. x86_64            */
}
```

Why it matters: the kernel release is the first thing you map against known local-privesc
advisories on a box you're assessing. We report it; we don't weaponise it.

> **Feature-test macros.** `#define _GNU_SOURCE` must come **before any `#include`**. It's
> what makes `nftw`, `strdup`, and `getline` visible. Put it at the very top of every
> `.c` that uses them — the solution does this consistently.

### `checks_users` — parse `/etc/passwd` (medium)

This is the M08 parsing pattern end to end: read line by line with `getline`, split the
colon-separated fields in place, and classify. The format (`passwd(5)`) is
`name:passwd:uid:gid:gecos:home:shell`.

```c
while ((len = getline(&line, &cap, fp)) != -1) {
    util_rstrip(line);
    if (line[0] == '\0' || line[0] == '#') continue;

    char *fields[7];
    if (split_passwd(line, fields) < 7) continue;   /* skip malformed lines */

    long uid = strtol(fields[2], NULL, 10);
    if (uid == 0)
        strvec_push(&uid0, fields[0]);              /* collect, sort, print later */
    ...
}
free(line);                                          /* getline() malloc'd it */
```

Note the ownership detail: `getline` allocates the line buffer for you and grows it as
needed; **you** free it once at the end. That is exactly the M07/M08 discipline.

Why it matters: a **second** UID 0 account is root-equivalent and a classic backdoor, so
it's a `[!]` warning; the normal `root` is just `[*]` info. Accounts with real login
shells are attack surface worth noting.

### `checks_suid` — walk the filesystem with `nftw` (harder)

`nftw(3)` walks a directory tree and hands your callback a `struct stat *` for each entry
— so you can test the SUID/SGID bits without a second `stat(2)`:

```c
#define _GNU_SOURCE
#include <ftw.h>

static int suid_cb(const char *path, const struct stat *sb,
                   int typeflag, struct FTW *ftwbuf) {
    (void)ftwbuf;
    if (typeflag == FTW_NS || typeflag == FTW_DNR) return FTW_CONTINUE; /* degrade */
    if (typeflag != FTW_F) return FTW_CONTINUE;
    if (sb->st_mode & (S_ISUID | S_ISGID))
        report_item(g_rep, is_common(util_basename(path)) ? SEV_INFO : SEV_NOTE,
                    "%s (uid %u)", path, (unsigned)sb->st_uid);
    return FTW_CONTINUE;
}
/* ... */
nftw("/usr/bin", suid_cb, 16, FTW_PHYS | FTW_ACTIONRETVAL);
```

Three things worth understanding here:

- **`FTW_PHYS`** means "don't follow symlinks" — you want the real file's bits.
- **`nftw` has no user-data parameter.** The callback reaches the report context through
  a file-scope `static report_t *g_rep`. That's acceptable here because the tool is
  single-threaded; we set it before the walk and clear it after. (A thread-safe design
  would use `nftw`'s GNU cousins or a different walker.)
- **We scan common bin dirs, not all of `/`.** Walking the whole filesystem is slow and
  noisy; the real cases live in `/usr/bin`, `/usr/sbin`, `/opt`, and friends. The
  world-writable check does walk deeper, so it caps its depth with `FTW_SKIP_SUBTREE`.

Why it matters: a SUID-root binary runs as root for anyone. A *non-standard* one (not in
our small "expected" list) gets a `[+]` note so it stands out for a closer look — the
kind of thing you'd check against GTFOBins-style notes on a box you're assessing.

### The rest, same shape

- **`checks_worldwritable`** — `nftw` again, depth-limited, flagging `S_IWOTH`. A
  world-writable directory *without* the sticky bit is a `[!]` (anyone can replace files);
  `/tmp` with its sticky bit is expected.
- **`checks_cron`** — read `/etc/crontab` and `/etc/cron.d/*` as crontab text, list the
  `cron.{hourly,daily,weekly,monthly}` scripts, and `[!]` warn on any cron file writable
  by group/other (writable job running as root = root code execution).
- **`checks_net`** — parse `/proc/net/tcp` and `/proc/net/tcp6` for `LISTEN` sockets
  (state `0A`), decoding the little-endian hex address, and flag anything bound to
  `0.0.0.0`/`[::]` as off-box attack surface. Then a **Module 10** flourish: a
  non-blocking `connect()` probe to a few common ports on `127.0.0.1` to confirm what
  actually accepts a connection. It only ever touches loopback.
- **`checks_env`** — split `PATH` by hand (so empty elements, which mean "current
  directory", are visible), and `[!]` warn on relative or world-writable entries that
  enable PATH hijacking. Note `LD_PRELOAD`/`LD_LIBRARY_PATH` if set.

Read the full, commented versions in `solution/src/` — each file opens with a comment
block stating *what it reads* and *why an assessor cares*.

## (e) Wire together `main`

`main` is deliberately thin. It parses args, picks the output stream, builds a
`report_t`, then walks a **registry** of checks:

```c
typedef struct {
    const char *name;
    const char *desc;
    void      (*run)(report_t *rep);
} check_t;

static const check_t CHECKS[] = {
    { "system",        "OS, kernel and distro",        check_system },
    { "users",         "accounts and shells",          check_users },
    { "suid",          "SUID/SGID binaries",           check_suid },
    { "worldwritable", "world-writable paths",         check_worldwritable },
    { "cron",          "scheduled jobs",               check_cron },
    { "net",           "listening TCP services",       check_net },
    { "env",           "PATH and environment",         check_env },
};
```

The main loop is then just:

```c
for (size_t i = 0; i < N_CHECKS; i++) {
    if (opts.only && !name_in_csv(opts.only, CHECKS[i].name)) continue;
    CHECKS[i].run(&rep);
}
```

That function-pointer table is the whole architecture in one place: `--list` iterates it,
`--only` filters it, and adding a check is one new row. The exit code reflects *findings*,
not *tool failure* — `1` if any warning printed, else `0` — so you can use `reconc` in a
script.

## (f) Build & test with `make` + `valgrind`

The `Makefile` (see `solution/Makefile`, fully annotated) gives you:

```sh
make            # ./reconc, -std=gnu11 -Wall -Wextra -O2
make debug      # -O0 -g3 for gdb
make asan       # -fsanitize=address,undefined
make clean
```

It compiles each `src/*.c` to a `.o` and links them, and tracks header dependencies
automatically with `-MMD -MP` so editing a `.h` rebuilds exactly the files that use it —
the M11 build hygiene, applied for real.

**Verify it's clean.** Three gates, all from M11:

```sh
make                                   # zero warnings — treat any warning as a bug
make debug && valgrind --leak-check=full --error-exitcode=1 ./reconc
make asan  && ./reconc                 # ASan/UBSan abort on any memory/UB error
```

If `valgrind` reports "definitely lost", find the `malloc` with no matching `free` — the
usual suspects are an early `return` that skips a cleanup, or a `getline`/`strdup` buffer
you forgot. `gdb` with a breakpoint on the check function tells you where.

## (g) Stretch goals

Pick a couple — they're the same ones the `exercises/` folder specifies with verification
steps:

1. **`--only` for groups / a `--exclude`.** You already have the registry; extend the
   filter.
2. **JSON output mode (`--format=json`).** This is where `strbuf_t` earns its keep: build
   each record in a growable buffer and escape strings properly. Diff against `jq .`.
3. **A `docker`/`lxd` group check.** Membership in `docker` is effectively root on many
   boxes — a textbook thing to flag.
4. **Capabilities.** Read file capabilities (`getcap`-style) as a modern cousin of SUID.
5. **Unit-testable parsers.** Pull `split_passwd` and the `/proc/net/tcp` decoder into
   pure functions and test them against fixture strings, no root required.
6. **A writable-by-me view.** Flag files owned by *other* users that *your* uid can still
   write — often more actionable than world-writable alone.

Keep every addition on the right side of the line: it must **report**, not **attack**.

## (h) Reflection + where to go next

You just built a real tool: multiple translation units, a clean build, graceful error
handling, no leaks, and a genuine use. That is the jump from "learning C" to "writing C".

Step back and notice the shape of what worked: a tiny shared library (`report`, `util`),
independent modules behind a one-function interface, a registry that makes the program
easy to grow, and a spec that kept scope honest. That pattern scales far past this tool.

**Where to go next:**

- **_The Linux Programming Interface_ (Michael Kerrisk).** The definitive book on the
  Linux/UNIX system-call API — `nftw`, sockets, `/proc`, process credentials, the lot.
  If you read one thing after this course, read this.
- **Secure coding.** CERT C Coding Standard and OWASP's C/C++ guidance: integer overflow,
  `TOCTOU` races (relevant to the stat-then-use pattern in our checks), safe string
  handling. Your checks are read-only, which sidesteps a lot — but know the hazards.
- **Read real tools for *ideas*, not to copy.** `LinEnum` and `linpeas` (shell) and tools
  like `lynis` enumerate far more than we do. Study *what* they check and *why* it matters
  to an assessor — then decide what belongs in a reporting tool. Use them as study
  references on hosts you're authorized to assess, and keep attribution honest if you
  borrow an idea.
- **Contribute.** Find an open-source CLI in C, read its `Makefile` and headers, fix a
  small bug. Reading other people's C is how the idioms become yours.

And keep the discipline this module is built on: **authorized targets only, report don't
exploit.** The same curiosity that finds a misconfiguration is the curiosity that fixes
it — stay on the side that's allowed to.

---

## Common mistakes / gotchas

- **Forgetting `#define _GNU_SOURCE` before includes.** You'll get implicit-declaration
  warnings for `nftw`, `strdup`, `getline`. It must be the first line, before any
  `#include`.
- **`%p` without a cast / `%zu` for the wrong type.** Cast pointers to `(void *)` for
  `%p`, and use `%zu` for `size_t`. `-Wformat` (part of `-Wall`) will catch mismatches —
  listen to it.
- **Not freeing the `getline` buffer.** `getline` mallocs once and reuses across calls;
  free it **after** the loop, exactly once. `valgrind` will show the leak if you don't.
- **Assuming files exist.** `/proc/net/tcp6`, `/etc/crontab`, `/opt` may be absent. Check
  every `fopen`/`opendir`/`nftw` return and degrade to a `report_skip`, never a crash.
- **`strtok` hiding empty fields.** For `PATH`, split by hand — `strtok` collapses the
  empty element that means "current directory", which is the dangerous case.
- **Treating the exit code as "did it run".** Our exit code means "found a warning".
  Document that, or a script will misread a clean run.
- **Scope creep into attack features.** The moment a change exploits rather than reports,
  it's out of scope for this tool. Re-read the spec's non-goals.

## Recap

- A spec and a one-way module layout make a multi-file C program tractable and growable.
- One shared output module gives consistent, severity-tagged, assessor-framed results.
- Each check is an independent `void check_x(report_t *)` that reads and reports, using
  M08 file I/O, M09 `/proc` and syscalls, and M10 sockets.
- A function-pointer registry turns `--list`, `--only`, and "add a check" into trivial
  changes.
- `make` + `valgrind` + sanitizers (M11) are how you *prove* it's clean, not hope it is.
- Everything stays **authorized, local, and reporting-only** — by design, not by accident.

## Exercises

See [`exercises/README.md`](exercises/README.md) for extension tasks (JSON output, a
`docker`-group check, a `--only` filter you harden, and unit-testable parsers), each with
verification notes. Build your own `reconc` first, then compare with
[`solution/`](solution/).

## Further reading

- `man 3 nftw`, `man 2 uname`, `man 2 stat`, `man 5 passwd`, `man 5 proc`,
  `man 2 getsockopt`, `man 3 getline`, `man 7 feature_test_macros`
- _The Linux Programming Interface_, Michael Kerrisk — the reference for everything this
  tool touches.
- The CERT C Coding Standard — secure-coding practices for C.
