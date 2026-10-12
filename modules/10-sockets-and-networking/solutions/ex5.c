/* ex5.c - (stretch) faster scan: many non-blocking connects watched by one poll().
 *
 * Approach: resolve the host once; process the range in batches of K. For each
 * port start a non-blocking connect; collect the in-progress sockets into a
 * struct pollfd array watching POLLOUT; one poll() waits for the whole batch;
 * then each ready socket's SO_ERROR says open vs. refused. Scales far better
 * than one-port-at-a-time because a batch costs ~one timeout, not K of them.
 *
 * Authorized targets only (localhost / lab / permitted hosts).
 *
 * Build: gcc -std=c11 -Wall -Wextra ex5.c -o ex5
 * Run:   ./ex5 <host> <lo> <hi> <timeout_ms> [batch]
 */
#define _POSIX_C_SOURCE 200809L
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>   /* htons */
#include <poll.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_BATCH 64

static int set_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        return -1;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static void set_port(struct sockaddr_storage *ss, int family, int port) {
    if (family == AF_INET) {
        ((struct sockaddr_in *) ss)->sin_port = htons((uint16_t) port);
    } else {
        ((struct sockaddr_in6 *) ss)->sin6_port = htons((uint16_t) port);
    }
}

int main(int argc, char **argv) {
    if (argc != 5 && argc != 6) {
        fprintf(stderr, "usage: %s <host> <lo> <hi> <timeout_ms> [batch]\n", argv[0]);
        return 1;
    }
    const char *host = argv[1];
    int lo = atoi(argv[2]);
    int hi = atoi(argv[3]);
    int ms = atoi(argv[4]);
    int batch = (argc == 6) ? atoi(argv[5]) : DEFAULT_BATCH;
    if (lo < 1 || hi > 65535 || lo > hi || ms <= 0 || batch <= 0) {
        fprintf(stderr, "bad arguments\n");
        return 1;
    }

    /* Resolve the host once; remember the first address (minus the port). */
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *res;
    int err = getaddrinfo(host, NULL, &hints, &res);
    if (err != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        return 1;
    }
    struct sockaddr_storage base;
    memset(&base, 0, sizeof base);
    memcpy(&base, res->ai_addr, res->ai_addrlen);
    int family = res->ai_family;
    socklen_t addrlen = res->ai_addrlen;
    freeaddrinfo(res);

    printf("scanning %s ports %d-%d (timeout %dms, batch %d)...\n",
           host, lo, hi, ms, batch);

    struct pollfd *pfds = malloc((size_t) batch * sizeof *pfds);
    int *ports = malloc((size_t) batch * sizeof *ports);
    int *open_ports = NULL;
    size_t open_count = 0, open_cap = 0;
    if (pfds == NULL || ports == NULL) {
        perror("malloc");
        free(pfds);
        free(ports);
        return 1;
    }

    for (int start = lo; start <= hi; start += batch) {
        int end = start + batch - 1;
        if (end > hi) {
            end = hi;
        }

        int n = 0;   /* in-progress sockets this batch */
        for (int port = start; port <= end; port++) {
            int fd = socket(family, SOCK_STREAM, 0);
            if (fd < 0) {
                continue;
            }
            if (set_nonblock(fd) < 0) {
                close(fd);
                continue;
            }
            struct sockaddr_storage ss = base;
            set_port(&ss, family, port);

            int rc = connect(fd, (struct sockaddr *) &ss, addrlen);
            if (rc == 0) {
                /* connected instantly: record and close now */
                if (open_count == open_cap) {
                    open_cap = open_cap ? open_cap * 2 : 16;
                    int *b = realloc(open_ports, open_cap * sizeof *open_ports);
                    if (b == NULL) { perror("realloc"); goto fail; }
                    open_ports = b;
                }
                open_ports[open_count++] = port;
                close(fd);
                continue;
            }
            if (errno != EINPROGRESS) {
                close(fd);
                continue;
            }
            pfds[n].fd = fd;
            pfds[n].events = POLLOUT;
            pfds[n].revents = 0;
            ports[n] = port;
            n++;
        }

        if (n > 0) {
            int rc = poll(pfds, (nfds_t) n, ms);
            if (rc < 0) {
                perror("poll");
                goto fail;
            }
            for (int i = 0; i < n; i++) {
                if (pfds[i].revents != 0) {
                    int soerr = 0;
                    socklen_t slen = sizeof soerr;
                    if (getsockopt(pfds[i].fd, SOL_SOCKET, SO_ERROR,
                                   &soerr, &slen) == 0 && soerr == 0) {
                        if (open_count == open_cap) {
                            open_cap = open_cap ? open_cap * 2 : 16;
                            int *b = realloc(open_ports,
                                             open_cap * sizeof *open_ports);
                            if (b == NULL) { perror("realloc"); goto fail; }
                            open_ports = b;
                        }
                        open_ports[open_count++] = ports[i];
                    }
                }
                close(pfds[i].fd);
            }
        }
    }

    if (open_count == 0) {
        printf("open: (none)\n");
    } else {
        printf("open: ");
        for (size_t i = 0; i < open_count; i++) {
            printf("%d%s", open_ports[i], (i + 1 < open_count) ? ", " : "\n");
        }
    }

    free(pfds);
    free(ports);
    free(open_ports);
    return 0;

fail:
    free(pfds);
    free(ports);
    free(open_ports);
    return 1;
}
