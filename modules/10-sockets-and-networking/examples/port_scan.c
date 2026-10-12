/* port_scan.c - a minimal sequential connect() port scanner with per-port timeout.
 *
 * AUTHORIZED USE ONLY: scan localhost, your own lab VMs, or hosts you have
 * written permission to test. This is ordinary connect()-based scanning - the
 * networking piece the capstone uses to report listening services.
 *
 * Build: gcc -std=c11 -Wall -Wextra port_scan.c -o port_scan
 * Run:   ./port_scan <host> <lo-port> <hi-port>    e.g. ./port_scan 127.0.0.1 20 100
 */
#define _POSIX_C_SOURCE 200809L
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
        if (flags < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
            close(fd);
            fd = -1;
            continue;
        }

        int rc = connect(fd, p->ai_addr, p->ai_addrlen);
        if (rc == 0) {
            break;
        }
        if (errno != EINPROGRESS) {
            close(fd);
            fd = -1;
            continue;
        }

        fd_set wset;
        FD_ZERO(&wset);
        FD_SET(fd, &wset);
        struct timeval tv = { timeout_ms / 1000, (timeout_ms % 1000) * 1000 };

        rc = select(fd + 1, NULL, &wset, NULL, &tv);
        if (rc <= 0) {
            close(fd);
            fd = -1;
            continue;
        }

        int soerr = 0;
        socklen_t slen = sizeof soerr;
        if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &soerr, &slen) < 0 || soerr != 0) {
            close(fd);
            fd = -1;
            continue;
        }
        break;
    }

    freeaddrinfo(res);
    return fd;
}

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
