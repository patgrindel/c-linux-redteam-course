/* ex4.c - port scanner with configurable timeout and an open-port summary.
 *
 * Approach: connect_timeout per port; open ports are appended to a growable
 * array (Module 07), printed as one comma-separated line at the end.
 *
 * Authorized targets only (localhost / lab / permitted hosts).
 *
 * Build: gcc -std=c11 -Wall -Wextra ex4.c -o ex4
 * Run:   ./ex4 <host> <lo> <hi> <timeout_ms>
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
    if (argc != 5) {
        fprintf(stderr, "usage: %s <host> <lo> <hi> <timeout_ms>\n", argv[0]);
        return 1;
    }
    const char *host = argv[1];
    int lo = atoi(argv[2]);
    int hi = atoi(argv[3]);
    int ms = atoi(argv[4]);
    if (lo < 1 || hi > 65535 || lo > hi || ms <= 0) {
        fprintf(stderr, "bad arguments\n");
        return 1;
    }

    printf("scanning %s ports %d-%d (timeout %dms)...\n", host, lo, hi, ms);

    int *open_ports = NULL;
    size_t count = 0, cap = 0;

    for (int port = lo; port <= hi; port++) {
        char portstr[6];
        snprintf(portstr, sizeof portstr, "%d", port);
        int fd = connect_timeout(host, portstr, ms);
        if (fd >= 0) {
            close(fd);
            if (count == cap) {
                cap = cap ? cap * 2 : 16;
                int *bigger = realloc(open_ports, cap * sizeof *open_ports);
                if (bigger == NULL) {
                    perror("realloc");
                    free(open_ports);
                    return 1;
                }
                open_ports = bigger;
            }
            open_ports[count++] = port;
        }
    }

    if (count == 0) {
        printf("open: (none)\n");
    } else {
        printf("open: ");
        for (size_t i = 0; i < count; i++) {
            printf("%d%s", open_ports[i], (i + 1 < count) ? ", " : "\n");
        }
    }

    free(open_ports);
    return 0;
}
