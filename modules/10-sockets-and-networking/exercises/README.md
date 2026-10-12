# Module 10 — Exercises

Build with `gcc -std=c11 -Wall -Wextra` and the `#define _POSIX_C_SOURCE 200809L` the
examples use (it exposes `getaddrinfo`, `socket`, etc. under `-std=c11`). Check every call.

> **Authorized use only.** Test against `127.0.0.1`, your own lab VMs, or hosts you have
> written permission to assess. Nothing here is for use against systems you don't control.

Reference solutions in [`solutions/`](../solutions).

---

## 1. Resolver with service names

**Goal:** `./ex1 <host> <service>` prints each resolved address *and* the numeric port,
where `<service>` may be a name (`http`, `ssh`) or a number (`80`).

**Constraints:**
- Pass both `host` and `service` to `getaddrinfo`.
- For each result, print the IP (via `inet_ntop`) and the port (read `sin_port`/`sin6_port`
  and convert with `ntohs`).
- Report `getaddrinfo` failure via `gai_strerror`.

**Verify:**
```
$ ./ex1 localhost ssh
127.0.0.1 port 22
::1 port 22
```

---

## 2. One-shot line client

**Goal:** `./ex2 <host> <port>` connects, sends one line read from stdin, prints whatever
comes back, and exits.

**Constraints:**
- Reuse a `connect_to` helper (resolve + socket + connect loop).
- `send_all` the line; then loop `recv` until it returns 0.
- Clean up the socket on every path.

**Verify (against a local echo-style service, or any listener):**
```
$ echo "ping" | ./ex2 127.0.0.1 7     # port 7 = echo, if enabled
ping
```

Starter: [`ex2_starter.c`](ex2_starter.c).

---

## 3. Banner grabber with timeout

**Goal:** Like `examples/banner_grab.c`, but give up if no banner arrives within N
milliseconds (argument), so a silent-but-open port doesn't hang you.

**Constraints:**
- Connect (blocking is fine here), then use `select` on the socket for **readability** with
  the timeout before calling `recv`.
- If the timeout fires, print `<host>:<port> open, no banner (timeout)`.

**Verify:**
```
$ ./ex3 127.0.0.1 22 1000
127.0.0.1:22 banner: SSH-2.0-OpenSSH_9.6p1 ...
$ ./ex3 127.0.0.1 80 1000      # HTTP won't greet first
127.0.0.1:80 open, no banner (timeout)
```

---

## 4. Port scanner with a configurable timeout

**Goal:** Extend `examples/port_scan.c` to take the per-port timeout (ms) as a 4th argument
and print a one-line summary of open ports at the end (comma-separated).

**Constraints:**
- Reuse `connect_timeout`.
- Validate the range (1–65535, lo ≤ hi) and the timeout (> 0).
- Collect open ports into a dynamic array (Module 07) and print them all at the end.

**Verify:**
```
$ ./ex4 127.0.0.1 20 100 300
scanning 127.0.0.1 ports 20-100 (timeout 300ms)...
open: 22, 80
```

Starter: [`ex4_starter.c`](ex4_starter.c).

---

## 5. (Stretch) Faster scan with `poll`

**Goal:** Launch up to K non-blocking connects at once and wait on them together with a
single `poll`, instead of one port at a time. Scan a range much faster.

**Constraints:**
- Keep a batch of pending sockets, each mid-connect; build a `struct pollfd` array watching
  `POLLOUT`; after `poll`, check each ready socket's `SO_ERROR`.
- Respect the timeout per batch; close every socket.
- Still authorized-targets only.

**Verify:** Same open-port results as exercise 4 for the same range, noticeably faster on a
wide range (e.g. 1–1024).
