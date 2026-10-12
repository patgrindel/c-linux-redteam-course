/* ex3.c - banner grabber that gives up after a read timeout.
 *
 * Approach: connect (blocking); select() the socket for readability with the
 * timeout before recv, so a silent-but-open port doesn't hang us.
 *
 * Build: gcc -std=c11 -Wall -Wextra ex3.c -o ex3
 * Run:   ./ex3 <host> <port> <timeout_ms>
 */
#define _POSIX_C_SOURCE 200809L
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int connect_to(const char *host, const char *port) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *res;
    int err = getaddrinfo(host, port, &hints, &res);
    if (err != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        return -1;
    }
    int fd = -1;
    for (struct addrinfo *p = res; p != NULL; p = p->ai_next) {
        fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd < 0) {
            continue;
        }
        if (connect(fd, p->ai_addr, p->ai_addrlen) == 0) {
            break;
        }
        close(fd);
        fd = -1;
    }
    freeaddrinfo(res);
    return fd;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: %s <host> <port> <timeout_ms>\n", argv[0]);
        return 1;
    }
    const char *host = argv[1];
    const char *port = argv[2];
    int ms = atoi(argv[3]);

    int fd = connect_to(host, port);
    if (fd < 0) {
        fprintf(stderr, "%s:%s connection failed\n", host, port);
        return 1;
    }

    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(fd, &rset);
    struct timeval tv = { ms / 1000, (ms % 1000) * 1000 };

    int rc = select(fd + 1, &rset, NULL, NULL, &tv);
    if (rc < 0) {
        perror("select");
        close(fd);
        return 1;
    }
    if (rc == 0) {
        printf("%s:%s open, no banner (timeout)\n", host, port);
        close(fd);
        return 0;
    }

    char buf[512];
    ssize_t n = recv(fd, buf, sizeof buf - 1, 0);
    if (n > 0) {
        buf[n] = '\0';
        buf[strcspn(buf, "\r\n")] = '\0';
        printf("%s:%s banner: %s\n", host, port, buf);
    } else if (n == 0) {
        printf("%s:%s closed by peer, no banner\n", host, port);
    } else {
        perror("recv");
    }

    close(fd);
    return 0;
}
