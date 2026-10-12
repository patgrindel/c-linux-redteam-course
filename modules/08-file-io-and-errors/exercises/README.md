# Module 08 — Exercises

Build each with `gcc -std=c11 -Wall -Wextra` (add `-D_POSIX_C_SOURCE=200809L` or the
`#define` in your source where you use `getline`). No warnings. Check every I/O call.

Reference solutions are in [`solutions/`](../solutions). Try each yourself first.

---

## 1. Line counter (`wc -l` clone)

**Goal:** Print the number of lines in a file given as `argv[1]`.

**Constraints:**
- Use streams (`fopen` + `fgetc` or `getline`). Count newline characters.
- If `argv[1]` is missing, print a usage message to `stderr` and return `1`.
- If the file can't be opened, `perror` and return `1`.

**Verify:**
```
$ printf 'a\nb\nc\n' > /tmp/t.txt
$ ./ex1 /tmp/t.txt
3
$ ./ex1 /nope ; echo "exit=$?"
/nope: No such file or directory
exit=1
```

Starter: [`ex1_starter.c`](ex1_starter.c).

---

## 2. Substring grep (`grep-lite`)

**Goal:** `./ex2 <pattern> <file>` prints every line of `<file>` that contains `<pattern>`
(plain substring, no regex), prefixed by its 1-based line number.

**Constraints:**
- Read line by line (`getline` recommended so long lines aren't truncated).
- Use `strstr` for the match.
- Strip the trailing newline before printing, then add your own.

**Verify:**
```
$ printf 'root:x:0\ndaemon:x:1\nuser:x:1000\n' > /tmp/p
$ ./ex2 root /tmp/p
1: root:x:0
$ ./ex2 x /tmp/p
1: root:x:0
2: daemon:x:1
3: user:x:1000
```

---

## 3. Login-shell users from `/etc/passwd`

**Goal:** Print the username and shell of every account whose shell is a real login shell
(i.e. **not** `/usr/sbin/nologin`, `/sbin/nologin`, `/bin/false`, or empty).

**Constraints:**
- Reuse the field-splitting approach from `examples/parse_passwd.c` (split on `:`, handle
  empty fields — do **not** use `strtok`).
- Accept an optional path argument; default to `/etc/passwd`.
- Skip blank/comment lines and malformed lines (report malformed ones to `stderr`).

**Verify:** On a typical box, `root` and your own user appear; `daemon`, `bin`, `nobody`
etc. do not.
```
$ ./ex3
root  /bin/bash
patrick  /bin/bash
```
This is a direct building block for the capstone's user-enumeration check.

Starter: [`ex3_starter.c`](ex3_starter.c).

---

## 4. Robust file copy (descriptors)

**Goal:** `./ex4 <src> <dst>` copies `<src>` to `<dst>` using raw descriptors
(`open`/`read`/`write`/`close`).

**Constraints:**
- Open `<dst>` with `O_WRONLY | O_CREAT | O_TRUNC` and mode `0644`.
- Use a `write_all` helper that loops over partial writes.
- Report any failed call with `perror`, close what you opened, return non-zero.
- Copy in chunks (e.g. 4096 bytes); do not read the whole file into memory.

**Verify:**
```
$ ./ex4 /etc/hostname /tmp/copy && diff /etc/hostname /tmp/copy && echo OK
OK
$ ./ex4 /nope /tmp/x ; echo "exit=$?"
/nope: No such file or directory
exit=1
```

---

## 5. `key=value` config parser

**Goal:** Read a config file of `key = value` lines and print them normalized as
`key=value` (one per line), skipping blanks and `#` comments.

**Constraints:**
- Trim leading/trailing spaces around key and value.
- Split on the **first** `=` only (values may contain `=`).
- Report a line with no `=` as an error to `stderr` (with its line number) and skip it.

**Verify:**
```
$ printf '# cfg\nhost = 10.0.0.5\nport=22\nbad line\n' > /tmp/c
$ ./ex5 /tmp/c
host=10.0.0.5
port=22
/tmp/c:4: no '=' found, skipping
```
