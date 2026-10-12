/* tcp_client.c - connect to host:port, send a minimal HTTP/1.0 request, print reply.
 *
 * connect_to() is the reusable resolve+socket+connect core reused by the banner
 * grabber and scanner. send_all() loops over partial sends.
 *
 * Build: gcc -std=c11 -Wall -Wextra tcp_client.c -o tcp_client
 * Run:   ./tcp_client [host] [port]     (defaults example.com 80; needs network)
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
