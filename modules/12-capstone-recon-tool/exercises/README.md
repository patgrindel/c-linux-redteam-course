# Module 12 — Exercises

These extend the capstone `reconc` tool. Build your own version from the module
[`README.md`](../README.md) first (or start from [`solution/`](../solution/)), then take
on these. Each task states the **goal**, the **constraints**, and **how to verify**.

> ## ⚠️ Authorized use only
> Everything here keeps `reconc` a local, reporting-only assessment aid. Do not add
> features that exploit, escalate privileges, evade detection, or touch hosts you are not
> explicitly authorized to assess. If a change crosses from *reporting* to *attacking*,
> it does not belong in this tool.

General ground rules for every exercise:

- Must still compile clean under `gcc -std=gnu11 -Wall -Wextra` (zero warnings).
- Must stay `valgrind`-clean and ASan-clean (`make asan && ./reconc`).
- A missing file or permission-denied degrades to a note, never a crash.

---

## Exercise 1 — `--only` groups and a matching `--exclude`

**Goal.** You already have `--only=a,b`. Add `--exclude=a,b` that runs every check
*except* the named ones, and make `--only`/`--exclude` mutually exclusive (error if both
are given).

**Constraints.**
- Reuse the existing `name_in_csv` helper; don't duplicate the registry.
- An unknown name in either list prints a warning to `stderr` (as `--only` already does)
  but is not a fatal error.

**Verify.**
```sh
./reconc --exclude=net,suid        # runs the other five checks
./reconc --only=net --exclude=net  # errors: "use only one of --only/--exclude"
./reconc --exclude=bogus           # warns about 'bogus', runs all checks
```
Confirm the section banners present match your expectation for each run.

---

## Exercise 2 — JSON output mode (`--format=json`)

**Goal.** Add `--format=text` (default) and `--format=json`. In JSON mode, emit one array
of finding objects: `{"check","severity","message"}`, plus a summary object.

**Constraints.**
- This is what `strbuf_t` (the Module 07 growable string in `report.c`) is for — build
  each record in a buffer and append.
- **Escape strings properly**: at minimum `"`, `\`, and control characters. Paths from
  the filesystem can contain surprising bytes.
- No colour or "why" detail lines in JSON mode — machine output stays pure data.
- Keep the text mode byte-for-byte unchanged.

**Verify.**
```sh
./reconc --format=json | jq .                 # must parse with zero errors
./reconc --format=json | jq '.findings[] | select(.severity=="warn")'
```
`jq` failing to parse means an escaping bug. Try it on a box with an oddly named file to
stress the escaper.

---

## Exercise 3 — `docker` / `lxd` group membership check

**Goal.** Add a `groups` check that reports whether the current user (or any user) is in
the `docker`, `lxd`, `sudo`, `wheel`, or `adm` groups.

**Constraints.**
- Parse `/etc/group` (format `name:passwd:gid:member,member,...`) the same way
  `checks_users` parses `/etc/passwd`. You may also use `getgroups(2)` for the current
  process.
- Flag `docker`/`lxd` membership as `[!]` — on many boxes it is effectively root — and
  include a one-line "why" detail.
- Register it in the `CHECKS[]` table so `--list` and `--only` pick it up automatically.

**Verify.**
```sh
./reconc --only=groups
./reconc --list                     # 'groups' appears with a description
id                                  # cross-check the groups it reports against `id`
```

---

## Exercise 4 — Unit-testable parsers

**Goal.** Make the parsing logic testable without root or a real system. Extract the pure
functions — `split_passwd`, the `/proc/net/tcp` line decoder, and the `os-release` value
extractor — so each takes a string in and returns structured data out, with no I/O.

**Constraints.**
- The pure functions must not call `fopen`/`getenv` — the caller passes the bytes.
- Add a tiny `tests/` with a `test_parse.c` that feeds known fixture strings and
  `assert()`s the results. Wire a `make test` target.
- No external test framework; `assert.h` is enough.

**Verify.**
```sh
make test        # builds and runs the tests; non-zero exit on any failed assert
```
Add a deliberately malformed fixture (a `/etc/passwd` line with only five fields, a
truncated `/proc/net/tcp` row) and confirm the parser rejects it rather than reading out
of bounds. Run the test binary under `valgrind` too.

---

## Exercise 5 — "Writable by me" view

**Goal.** Alongside world-writable, flag files/dirs owned by a *different* user that the
*current* uid can nonetheless write (via group membership or ownership quirks) — often
more actionable than world-writable alone.

**Constraints.**
- Use `stat(2)` plus the process's real uid/gids (`getuid`, `getgid`, `getgroups`).
- Read-only: you may `stat` and test bits, but never actually open a file for writing to
  "prove" it.
- Keep output bounded (reuse the hit-cap pattern from `checks_worldwritable`).

**Verify.**
```sh
# As a non-root user, create a file your group can write, then:
./reconc --only=worldwritable -v
```
Confirm it appears in your new view but a root-only file does not. Re-run under ASan.

---

## Exercise 6 — Capabilities (modern SUID cousin)

**Goal.** Report files that carry Linux file capabilities (e.g. `cap_net_raw`,
`cap_setuid`) — a finer-grained privilege mechanism than SUID that assessors increasingly
care about.

**Constraints.**
- Read capabilities with the `libcap` API (`cap_get_file`) *or*, to avoid a dependency,
  by reading the `security.capability` extended attribute with `getxattr(2)` and decoding
  it. State which you chose and why in a comment.
- If the capability API/headers aren't present, the check should degrade to a single
  skip note — don't break the build on a box without `libcap`.
- Document any new link flag in the `Makefile` (e.g. `-lcap`) with a comment.

**Verify.**
```sh
sudo setcap cap_net_raw+ep ./some_test_binary   # on a box you own
./reconc --only=caps
getcap -r /usr/bin 2>/dev/null                   # cross-check a few results
```

---

## Submitting / self-check

For any exercise you finish, confirm all four gates before calling it done:

```sh
make                                              # 0 warnings
make debug && valgrind --leak-check=full --error-exitcode=1 ./reconc
make asan  && ./reconc
./reconc --help                                   # authorized-use notice still present
```
