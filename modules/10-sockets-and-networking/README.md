# Module 10 — Networking with Sockets

The Berkeley sockets API: resolving names, opening TCP connections, sending and receiving
bytes, grabbing a service banner, and — with non-blocking connects and `select`/`poll` —
building a small, timeout-driven port scanner for hosts you are authorized to test.

## Where we are

Module 09 had you talk to the kernel to manage processes. A socket is just another kind of
file descriptor (Module 08's `read`/`write` work on it), but it speaks to the network. The
API has a few moving parts; once you've seen a working TCP client the rest is variations.

## Learning objectives

By the end you can:

- Create a socket with `socket(2)`, connect with `connect(2)`, and move bytes with
  `send(2)`/`recv(2)` (or `read`/`write`) and `close(2)`.
- Fill in `struct sockaddr_in`, resolve hostnames with `getaddrinfo(3)`, and get byte order
  right with `htons`/`htonl`.
- Write a TCP connect client and grab a service banner.
- Do a **non-blocking** connect with a timeout using `select`/`poll`.
- Build a minimal connect-based port scanner over a port range on an authorized host.

> **Authorized use only.** Everything here is ordinary `connect()`-based networking — the
> same thing your browser does. Point the scanner at `127.0.0.1`, your own lab VMs, or
> hosts you have **written permission** to test. There is no raw-packet, stealth, or
> evasion content here, by design: this is an assessment aid, not an attack tool.

---

## The shape of the API

A TCP client is always the same five steps:

1. `getaddrinfo` — turn `"example.com"` + `"80"` into one or more ready-to-use addresses.
2. `socket` — create an endpoint, get back a file descriptor.
3. `connect` — establish the TCP connection to the address.
4. `send`/`recv` (or `write`/`read`) — exchange bytes.
5. `close` — done.

All of these need the POSIX networking headers and the `_POSIX_C_SOURCE` feature-test macro
under `-std=c11` (it exposes `getaddrinfo`, `struct addrinfo`, `socket`, etc.):

```c
#define _POSIX_C_SOURCE 200809L
#include <sys/types.h>
#include <sys/socket.h>   /* socket, connect, send, recv */
#include <netdb.h>        /* getaddrinfo, struct addrinfo */
#include <netinet/in.h>   /* struct sockaddr_in */
#include <arpa/inet.h>    /* htons, htonl, inet_ntop */
#include <unistd.h>       /* close */
```

---

## Byte order: `htons` / `htonl`

Networks are **big-endian** ("network byte order"); most CPUs you'll use are little-endian
("host byte order"). Any multi-byte number that goes *on the wire* — notably port numbers
and raw IPv4 addresses — must be converted:

- `htons` — **h**ost **to** **n**etwork, **s**hort (16-bit, e.g. a port).
- `htonl` — host to network, long (32-bit, e.g. an IPv4 address).
- `ntohs` / `ntohl` — the reverse, for values you read off the wire.

[`examples/byteorder.c`](examples/byteorder.c) makes it concrete:

```c
#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>

int main(void) {
    uint16_t port = 443;
    uint16_t net  = htons(port);
    printf("port 443: host=0x%04x network=0x%04x\n", port, net);

    uint32_t host_ip = 0x7f000001;        /* 127.0.0.1 as a 32-bit number */
    uint32_t net_ip  = htonl(host_ip);
    printf("ip 127.0.0.1: host=0x%08x network=0x%08x\n", host_ip, net_ip);
    return 0;
}
```

```
$ gcc -std=c11 -Wall -Wextra examples/byteorder.c -o byteorder
$ ./byteorder
port 443: host=0x01bb network=0xbb01
ip 127.0.0.1: host=0x7f000001 network=0x0100007f
```

On a little-endian machine the bytes are visibly swapped. On a (rare) big-endian machine
these macros are no-ops — which is the whole point: write the macro and your code is
correct either way. Forgetting `htons` on a port is the single most common socket bug: you
connect to the wrong port (e.g. 443 → 47873) and wonder why nothing answers.

---

## Resolving names: `getaddrinfo`

`getaddrinfo` is the modern, IPv4/IPv6-agnostic way to turn a host and service into
socket-ready address structures. It returns a linked list of `struct addrinfo`; you walk
it and use the first one that works. Free the list with `freeaddrinfo`.

[`examples/resolve.c`](examples/resolve.c) prints every address a name resolves to:

```c
#define _POSIX_C_SOURCE 200809L
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <hostname>\n", argv[0]);
        return 1;
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;       /* IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;     /* TCP */

    struct addrinfo *res;
    int err = getaddrinfo(argv[1], NULL, &hints, &res);
    if (err != 0) {
        /* getaddrinfo has its OWN error function, not errno */
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        return 1;
    }

    for (struct addrinfo *p = res; p != NULL; p = p->ai_next) {
        char ip[INET6_ADDRSTRLEN];
        void *addr;
        if (p->ai_family == AF_INET) {
            addr = &((struct sockaddr_in *) p->ai_addr)->sin_addr;
        } else {
            addr = &((struct sockaddr_in6 *) p->ai_addr)->sin6_addr;
        }
        if (inet_ntop(p->ai_family, addr, ip, sizeof ip) != NULL) {
            printf("%s (%s)\n", ip, p->ai_family == AF_INET ? "IPv4" : "IPv6");
        }
    }

    freeaddrinfo(res);
    return 0;
}
```

```
$ gcc -std=c11 -Wall -Wextra examples/resolve.c -o resolve
$ ./resolve localhost
127.0.0.1 (IPv4)
::1 (IPv6)
$ ./resolve example.com
93.184.216.34 (IPv4)
2606:2800:220:1:248:1893:25c8:1946 (IPv6)
```

Two things to internalize: `getaddrinfo` reports errors through its **return value** and
`gai_strerror`, *not* `errno`. And `inet_ntop` turns a packed address back into text
(the inverse, `inet_pton`, parses text into packed form).

---

## A TCP connect client

Now put it together. [`examples/tcp_client.c`](examples/tcp_client.c) connects to a
host/port, sends a minimal HTTP request, and prints the response. The helper
`connect_to(host, port)` is the reusable core you'll see again in the banner grabber and
scanner.

```c
#define _POSIX_C_SOURCE 200809L
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Resolve host:port, connect, return a connected fd (or -1). */
static int connect_to(const char *host, const char *port) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *res;
    int err = getaddrinfo(host, port, &hints, &res);
    if (err != 0) {
        fprintf(stderr, "getaddrinfo(%s:%s): %s\n", host, port, gai_strerror(err));
        return -1;
    }

    int fd = -1;
    for (struct addrinfo *p = res; p != NULL; p = p->ai_next) {
        fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd < 0) {
            continue;                        /* try the next address */
        }
        if (connect(fd, p->ai_addr, p->ai_addrlen) == 0) {
            break;                           /* connected! */
        }
        close(fd);                           /* this address failed; clean up and retry */
        fd = -1;
    }

    freeaddrinfo(res);
    return fd;
}

/* send() may send fewer bytes than asked; loop until it's all out. */
static int send_all(int fd, const char *buf, size_t len) {
    size_t off = 0;
    while (off < len) {
        ssize_t n = send(fd, buf + off, len - off, 0);
        if (n < 0) {
            return -1;
        }
        off += (size_t) n;
    }
    return 0;
}

int main(int argc, char **argv) {
    const char *host = (argc > 1) ? argv[1] : "example.com";
    const char *port = (argc > 2) ? argv[2] : "80";

    int fd = connect_to(host, port);
    if (fd < 0) {
        fprintf(stderr, "could not connect to %s:%s\n", host, port);
        return 1;
    }
    printf("connected to %s:%s\n", host, port);

    char req[256];
    int req_len = snprintf(req, sizeof req,
                           "GET / HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n",
                           host);
    if (send_all(fd, req, (size_t) req_len) < 0) {
        perror("send");
        close(fd);
        return 1;
    }

    char buf[4096];
    ssize_t n;
    /* recv returns 0 when the peer closes the connection (end of data). */
    while ((n = recv(fd, buf, sizeof buf - 1, 0)) > 0) {
        buf[n] = '\0';
        fputs(buf, stdout);
    }
    if (n < 0) {
        perror("recv");
    }

    close(fd);
    return 0;
}
```

```
$ gcc -std=c11 -Wall -Wextra examples/tcp_client.c -o tcp_client
$ ./tcp_client example.com 80
connected to example.com:80
HTTP/1.0 200 OK
Content-Type: text/html; charset=UTF-8
...
<!doctype html>
...
```

Key points:

- Loop over `getaddrinfo`'s results and use the first address that both `socket`s and
  `connect`s. This is what makes a client "just work" on IPv4 or IPv6.
- `recv` returning **0** means the peer closed the connection — that's your clean EOF, not
  an error. A return of `-1` is an error (check `errno`).
- `send`, like `write`, can be partial; `send_all` loops. (`recv` returning less than you
  asked for is normal and fine — TCP is a stream, not messages.)

---

## Grabbing a service banner

Many services announce themselves the moment you connect (SMTP, FTP, SSH, many others).
Reading that first line — the **banner** — tells you what software and often what version
is listening. On a host you're assessing, an outdated banner (e.g. an old OpenSSH or
vsftpd) is a lead worth noting.

[`examples/banner_grab.c`](examples/banner_grab.c) reuses `connect_to`, then reads once:

```c
    int fd = connect_to(host, port);
    if (fd < 0) {
        return 1;
    }
    char buf[512];
    ssize_t n = recv(fd, buf, sizeof buf - 1, 0);
    if (n > 0) {
        buf[n] = '\0';
        buf[strcspn(buf, "\r\n")] = '\0';   /* first line only */
        printf("%s:%s banner: %s\n", host, port, buf);
    } else if (n == 0) {
        printf("%s:%s connected but sent no banner\n", host, port);
    } else {
        perror("recv");
    }
    close(fd);
```

Test it against a local service you run (here, an SSH daemon on port 22):

```
$ gcc -std=c11 -Wall -Wextra examples/banner_grab.c -o banner_grab
$ ./banner_grab 127.0.0.1 22       # requires sshd listening on localhost
127.0.0.1:22 banner: SSH-2.0-OpenSSH_9.6p1 Ubuntu-3ubuntu13.5
```

(No listener on that port? You'll get "connection refused". Some services, like HTTP, say
nothing until *you* speak first — for those you must send a request before `recv`.)

---

## Timeouts: non-blocking connect + `select`

A plain `connect` to a dead host can block for a long time (until the OS gives up — often
tens of seconds). For a scanner you want to decide "open, closed, or filtered" in, say, one
second. The technique:

1. Put the socket in **non-blocking** mode with `fcntl(fd, F_SETFL, O_NONBLOCK)`.
2. Call `connect`. It returns immediately with `-1` and `errno == EINPROGRESS`.
3. `select`/`poll` the socket for **writability** with a timeout.
4. When it becomes writable, check `SO_ERROR` with `getsockopt` to learn whether the
   connect actually succeeded (0) or was refused (e.g. `ECONNREFUSED`).

[`examples/nonblock_connect.c`](examples/nonblock_connect.c) wraps this into
`connect_timeout(host, port, ms)`:

```c
#define _POSIX_C_SOURCE 200809L
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Try to connect within `timeout_ms`. Returns a connected fd, or -1. */
static int connect_timeout(const char *host, const char *port, int timeout_ms) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *res;
    if (getaddrinfo(host, port, &hints, &res) != 0) {
        return -1;
    }

    int fd = -1;
    for (struct addrinfo *p = res; p != NULL; p = p->ai_next) {
        fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd < 0) {
            continue;
        }

        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);   /* non-blocking */

        int rc = connect(fd, p->ai_addr, p->ai_addrlen);
        if (rc == 0) {
            break;                                /* connected immediately (rare) */
        }
        if (errno != EINPROGRESS) {
            close(fd);
            fd = -1;
            continue;                             /* refused right away */
        }

        /* Wait for the socket to become writable, or time out. */
        fd_set wset;
        FD_ZERO(&wset);
        FD_SET(fd, &wset);
        struct timeval tv = { timeout_ms / 1000, (timeout_ms % 1000) * 1000 };

        rc = select(fd + 1, NULL, &wset, NULL, &tv);
        if (rc <= 0) {                            /* 0 = timeout, -1 = error */
            close(fd);
            fd = -1;
            continue;
        }

        /* Writable: did the connect succeed or fail? Ask SO_ERROR. */
        int soerr = 0;
        socklen_t slen = sizeof soerr;
        if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &soerr, &slen) < 0 || soerr != 0) {
            close(fd);
            fd = -1;
            continue;
        }
        break;                                    /* success */
    }

    freeaddrinfo(res);
    return fd;
}

int main(int argc, char **argv) {
    const char *host = (argc > 1) ? argv[1] : "127.0.0.1";
    const char *port = (argc > 2) ? argv[2] : "22";

    int fd = connect_timeout(host, port, 1000);   /* 1 second */
    if (fd < 0) {
        printf("%s:%s closed/filtered (or timed out)\n", host, port);
        return 1;
    }
    printf("%s:%s open\n", host, port);
    close(fd);
    return 0;
}
```

```
$ gcc -std=c11 -Wall -Wextra examples/nonblock_connect.c -o nonblock_connect
$ ./nonblock_connect 127.0.0.1 22
127.0.0.1:22 open
$ ./nonblock_connect 127.0.0.1 81
127.0.0.1:81 closed/filtered (or timed out)
```

The `SO_ERROR` check in step 4 is essential and easy to forget: a socket becoming writable
only means the connect *attempt finished* — success and refusal both wake `select`. You
must read `SO_ERROR` to tell them apart.

---

## A minimal port scanner

Put the timeout connect in a loop over a port range. [`examples/port_scan.c`](examples/port_scan.c)
scans `host` from `lo` to `hi` and prints which ports accept a connection. It's sequential
and uses a short per-port timeout — simple, readable, and perfectly adequate for assessing
a handful of hosts you're authorized to test.

```c
int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: %s <host> <lo-port> <hi-port>\n", argv[0]);
        return 1;
    }
    const char *host = argv[1];
    int lo = atoi(argv[2]);
    int hi = atoi(argv[3]);
    if (lo < 1 || hi > 65535 || lo > hi) {
        fprintf(stderr, "bad port range\n");
        return 1;
    }

    printf("scanning %s ports %d-%d...\n", host, lo, hi);
    int open_count = 0;
    for (int port = lo; port <= hi; port++) {
        char portstr[6];
        snprintf(portstr, sizeof portstr, "%d", port);
        int fd = connect_timeout(host, portstr, 500);   /* 0.5s per port */
        if (fd >= 0) {
            printf("  %5d open\n", port);
            open_count++;
            close(fd);
        }
    }
    printf("done: %d open port(s)\n", open_count);
    return 0;
}
```

```
$ gcc -std=c11 -Wall -Wextra examples/port_scan.c -o port_scan
$ ./port_scan 127.0.0.1 20 100
scanning 127.0.0.1 ports 20-100...
     22 open
     80 open
done: 2 open port(s)
```

This is the networking piece the capstone uses to report listening services on the host
you're assessing. To go faster you'd fire many non-blocking connects at once and wait on
them all with a single `poll` — a natural extension once you're comfortable with the
one-at-a-time version. (`poll(2)` takes an array of `struct pollfd`, which scales better
than `select`'s `fd_set` when you have many sockets.)

---

## Common mistakes / gotchas

- **Forgetting `htons` on the port** (or `htonl` on an address). Everything "works" until
  you realize you connected to a scrambled port number.
- **Treating `recv == 0` as an error.** Zero means the peer closed cleanly — that's your
  EOF. Only `-1` is an error.
- **Assuming `send`/`recv` move everything in one call.** TCP is a byte stream: loop your
  sends; treat each `recv` as "some bytes, maybe not all".
- **Checking `errno` after `getaddrinfo`.** It doesn't use `errno`; use its return value
  and `gai_strerror`.
- **Skipping the `SO_ERROR` check** after a non-blocking connect reports writable. You'll
  mark refused ports as open.
- **Leaking sockets.** Every `socket` needs a `close`, including the failed-connect path in
  the resolve loop — or a long scan hits `EMFILE`.
- **Expecting a banner from HTTP.** Web servers wait for your request first; send before
  you `recv`.
- **Scanning hosts you don't own or lack permission to test.** Don't. Keep it to localhost,
  your lab, and authorized targets.

## Recap

- A socket is a file descriptor for the network; a TCP client is
  `getaddrinfo` → `socket` → `connect` → `send`/`recv` → `close`.
- Multi-byte values on the wire use network byte order: `htons`/`htonl` (and `ntoh*` back).
- `getaddrinfo` resolves host+service to ready-made addresses and reports errors via
  `gai_strerror`, not `errno`.
- A service's first reply is often a version **banner** worth recording.
- Non-blocking connect + `select`/`poll` + an `SO_ERROR` check gives you connects with a
  timeout — the basis of a connect-based port scanner.
- Keep all of this to authorized hosts.

## Exercises

See [`exercises/README.md`](exercises/README.md).

## Further reading

- `man` pages: `socket(2)`, `connect(2)`, `send(2)`, `recv(2)`, `getaddrinfo(3)`,
  `gai_strerror(3)`, `inet_ntop(3)`, `htons(3)`, `select(2)`, `poll(2)`, `getsockopt(2)`,
  `fcntl(2)`, `ip(7)`, `tcp(7)`.
- Beej's *Guide to Network Programming* — the classic, approachable sockets tutorial.
- Michael Kerrisk, *The Linux Programming Interface*, chapters 56–61 (sockets).
