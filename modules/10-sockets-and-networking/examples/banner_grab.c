/* banner_grab.c - connect to host:port and print the first line the service sends.
 *
 * Works for services that greet first (SSH, SMTP, FTP). For HTTP you must send a
 * request before recv (see tcp_client.c).
 *
 * Build: gcc -std=c11 -Wall -Wextra banner_grab.c -o banner_grab
 * Run:   ./banner_grab <host> <port>     e.g. ./banner_grab 127.0.0.1 22
 */
#define _POSIX_C_SOURCE 200809L
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
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
        fprintf(stderr, "getaddrinfo(%s:%s): %s\n", host, port, gai_strerror(err));
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
    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        return 1;
    }
    const char *host = argv[1];
    const char *port = argv[2];

    int fd = connect_to(host, port);
    if (fd < 0) {
        fprintf(stderr, "%s:%s connection failed\n", host, port);
        return 1;
    }

    char buf[512];
    ssize_t n = recv(fd, buf, sizeof buf - 1, 0);
    if (n > 0) {
        buf[n] = '\0';
        buf[strcspn(buf, "\r\n")] = '\0';     /* first line only */
        printf("%s:%s banner: %s\n", host, port, buf);
    } else if (n == 0) {
        printf("%s:%s connected but sent no banner\n", host, port);
    } else {
        perror("recv");
    }

    close(fd);
    return 0;
}
