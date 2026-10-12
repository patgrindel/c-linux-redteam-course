/* ex1.c - resolver that prints address + numeric port for a host/service pair.
 *
 * Approach: getaddrinfo with both host and service; for each result pull the
 * address (inet_ntop) and the port out of the right sockaddr struct (ntohs).
 *
 * Build: gcc -std=c11 -Wall -Wextra ex1.c -o ex1
 * Run:   ./ex1 <host> <service>
 */
#define _POSIX_C_SOURCE 200809L
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <service>\n", argv[0]);
        return 1;
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *res;
    int err = getaddrinfo(argv[1], argv[2], &hints, &res);
    if (err != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        return 1;
    }

    for (struct addrinfo *p = res; p != NULL; p = p->ai_next) {
        char ip[INET6_ADDRSTRLEN];
        void *addr;
        unsigned port;
        if (p->ai_family == AF_INET) {
            struct sockaddr_in *s = (struct sockaddr_in *) p->ai_addr;
            addr = &s->sin_addr;
            port = ntohs(s->sin_port);
        } else {
            struct sockaddr_in6 *s = (struct sockaddr_in6 *) p->ai_addr;
            addr = &s->sin6_addr;
            port = ntohs(s->sin6_port);
        }
        if (inet_ntop(p->ai_family, addr, ip, sizeof ip) != NULL) {
            printf("%s port %u\n", ip, port);
        }
    }

    freeaddrinfo(res);
    return 0;
}
