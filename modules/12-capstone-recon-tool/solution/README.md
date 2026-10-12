# reconc — reference implementation

`reconc` is the capstone tool for the course: a small, multi-file Linux CLI that
enumerates the **local** host and reports its security posture, the way you would
at the start of an authorized, OSCP-style assessment.

> ## ⚠️ Authorized use only
> This tool inspects the host it runs on and reports what it finds. It does **not**
> exploit anything, does **not** escalate privileges, does **not** try to hide, and
> does **not** touch third-party systems. The one piece that uses the network only
> ever connects to `127.0.0.1` (your own loopback). Run it **only** on a machine you
> own or have **explicit written permission** to assess.

This is the finished solution. If you are working through the module, build your own
version first using `../README.md` — then compare.

## Build

```sh
make            # optimised build -> ./reconc
make debug      # -O0 -g3 for gdb
make asan       # AddressSanitizer + UBSan build
make clean
```

Requires `gcc` (or clang) and `make` on Linux. The code targets
`gcc -std=gnu11 -Wall -Wextra` and builds warning-free.

## Run

```sh
./reconc                    # run every check against the local host
./reconc --list             # show the available checks
./reconc --only=suid,users  # run just two checks
./reconc -v                 # verbose: also show skipped/unreadable scans
./reconc -o report.txt      # write the report to a file (no colour)
./reconc --help             # full usage, including the authorized-use notice
```

Findings are tagged by severity, framed for an assessor reading the output:

| Tag   | Meaning                                                          |
|-------|-----------------------------------------------------------------|
| `[*]` | **info** — a neutral fact about the host                        |
| `[+]` | **note** — worth a second look during the assessment            |
| `[!]` | **warning** — a posture weakness that commonly matters          |

The process exit code is `1` if any `[!]` warning was printed, else `0` — handy when
calling `reconc` from a script.

## What each check looks at, and why it matters

| Check           | Inspects                                   | Why an assessor cares                                             |
|-----------------|--------------------------------------------|-------------------------------------------------------------------|
| `system`        | `uname(2)`, `/proc/version`, `/etc/os-release` | Kernel/distro version maps to known local-privesc advisories. |
| `users`         | `/etc/passwd`                              | Extra UID 0 accounts are root-equivalent; shells = attack surface.|
| `suid`          | SUID/SGID files in common bin dirs         | A vulnerable SUID-root binary is a classic privesc vector.        |
| `worldwritable` | world-writable files/dirs in key paths     | Anyone can tamper with them; non-sticky WW dirs are worse.        |
| `cron`          | `/etc/crontab`, `/etc/cron.d`, `cron.*`    | A writable job that runs as root = root code execution.           |
| `net`           | `/proc/net/tcp{,6}` + localhost probe      | Services on all interfaces are remote attack surface.             |
| `env`           | `PATH`, `LD_PRELOAD`, `LD_LIBRARY_PATH`    | Writable/relative PATH entries enable PATH hijacking.             |

## Sample output (trimmed)

```
reconc 1.0.0 -- local host posture assessment
AUTHORIZED USE ONLY: run only against a host you own or are permitted to assess.
This tool reports posture; it does not exploit or change the system.

== System & kernel ==
    Hostname: lab-box
    Kernel: 5.15.0-91-generic
    Arch: x86_64
    System: Linux #101-Ubuntu SMP ...
    Distro: Ubuntu 22.04.3 LTS
  [*] Kernel build: Linux version 5.15.0-91-generic (buildd@...) ...
      Cross-check the release against known local-privesc advisories for this distro.

== Users & accounts ==
  [*] UID 0 account: root
  [+] Login shell: root (uid 0, /bin/bash)
  [+] Login shell: dev (uid 1000, /bin/bash)
      Interactive accounts are attack surface; note them for authorized credential checks.

== SUID / SGID binaries ==
  [*] -rwsr-xr-x /usr/bin/sudo (uid 0)
  [*] -rwsr-xr-x /usr/bin/passwd (uid 0)
  [+] -rwsr-xr-x /usr/local/bin/backup-helper (uid 0)
      Check any [+] entry against GTFOBins-style known-abuse notes before relying on it.

== World-writable paths ==
  [!] drwxrwxrwx /opt/shared (NO sticky bit)
      Writable files that root or a service reads/executes are the ones to prioritise.

== Scheduled jobs (cron) ==
  [+] crontab: 17 *	* * *	root	cd / && run-parts --report /etc/cron.hourly
  [*] cron.daily/logrotate
  [!] cron file is non-root-writable: -rw-rw-r-- /etc/cron.d/backup
      If this runs as root, a writable entry = root code execution.

== Listening TCP services ==
  [*] tcp  127.0.0.1:631   unknown   [loopback]
  [!] tcp  0.0.0.0:22      ssh       [all interfaces]
      A service on all interfaces is reachable off-box; the same service on loopback is local-only.
  [+] connect ok: 127.0.0.1:22 (ssh)

== Environment & PATH ==
    PATH: /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
  [!] PATH element is world-writable: /usr/local/bin
      Anyone can plant a binary here that shadows a real command.

== Summary ==
    1 info, 4 notes, 3 warnings
      Counts are a starting point, not a score -- read the findings in context.
```

Your real output will differ — that's the point. (The findings above are illustrative;
a stock box will show far fewer warnings.)

## Memory hygiene

Everything that is `malloc`'d is `free`'d. Verify with either:

```sh
make debug && valgrind --leak-check=full --error-exitcode=1 ./reconc
make asan  && ./reconc        # ASan/UBSan abort on any error
```

## File layout

```
solution/
├── Makefile
├── README.md          (this file)
└── src/
    ├── main.c                 entry point + check registry
    ├── args.c / args.h        CLI parsing
    ├── report.c / report.h    output helpers + strbuf/strvec (from M07)
    ├── util.c / util.h        small shared helpers
    ├── version.h              program name + version
    ├── checks_system.c/.h
    ├── checks_users.c/.h
    ├── checks_suid.c/.h
    ├── checks_worldwritable.c/.h
    ├── checks_cron.c/.h
    ├── checks_net.c/.h
    └── checks_env.c/.h
```
