/* ex2_starter.c - one-shot line client. Fill in the TODOs.
 *
 * Build: gcc -std=c11 -Wall -Wextra ex2_starter.c -o ex2
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

    /* TODO: send_all(fd, line, strlen(line)); then loop recv printing until 0. */
    (void) line;

    close(fd);
    return 0;
}
