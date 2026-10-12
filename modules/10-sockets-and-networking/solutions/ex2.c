/* ex2.c - one-shot line client: send one stdin line, print the reply.
 *
 * Approach: connect_to helper; send_all the line; recv loop until the peer
 * closes (recv == 0).
 *
 * Build: gcc -std=c11 -Wall -Wextra ex2.c -o ex2
 * Run:   echo "hello" | ./ex2 <host> <port>
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
    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        return 1;
    }
    int fd = connect_to(argv[1], argv[2]);
    if (fd < 0) {
        fprintf(stderr, "connect failed\n");
        return 1;
    }

    char line[1024];
    if (fgets(line, sizeof line, stdin) == NULL) {
        close(fd);
        return 0;
    }
    if (send_all(fd, line, strlen(line)) < 0) {
        perror("send");
        close(fd);
        return 1;
    }

    char buf[4096];
    ssize_t n;
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
