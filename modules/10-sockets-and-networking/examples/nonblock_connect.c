/* nonblock_connect.c - connect with a timeout using a non-blocking socket + select.
 *
 * Steps: set O_NONBLOCK; connect returns EINPROGRESS; select() for writability
 * with a timeout; then getsockopt(SO_ERROR) to tell "connected" from "refused".
 *
 * Build: gcc -std=c11 -Wall -Wextra nonblock_connect.c -o nonblock_connect
 * Run:   ./nonblock_connect [host] [port]    (defaults 127.0.0.1 22)
 */
#define _POSIX_C_SOURCE 200809L
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
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
            break;                            /* connected immediately (rare) */
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
        if (rc <= 0) {                        /* 0 = timeout, -1 = error */
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
        break;                                /* success */
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
