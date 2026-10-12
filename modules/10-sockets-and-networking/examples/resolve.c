/* resolve.c - print every address a hostname resolves to (getaddrinfo).
 *
 * getaddrinfo reports errors via its return value + gai_strerror, NOT errno.
 *
 * Build: gcc -std=c11 -Wall -Wextra resolve.c -o resolve
 * Run:   ./resolve <hostname>
 */
#define _POSIX_C_SOURCE 200809L
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <hostname>\n", argv[0]);
        return 1;
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;       /* IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;     /* TCP */

    struct addrinfo *res;
    int err = getaddrinfo(argv[1], NULL, &hints, &res);
    if (err != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        return 1;
    }

    for (struct addrinfo *p = res; p != NULL; p = p->ai_next) {
        char ip[INET6_ADDRSTRLEN];
        void *addr;
        if (p->ai_family == AF_INET) {
            addr = &((struct sockaddr_in *) p->ai_addr)->sin_addr;
        } else {
            addr = &((struct sockaddr_in6 *) p->ai_addr)->sin6_addr;
        }
        if (inet_ntop(p->ai_family, addr, ip, sizeof ip) != NULL) {
            printf("%s (%s)\n", ip, p->ai_family == AF_INET ? "IPv4" : "IPv6");
        }
    }

    freeaddrinfo(res);
    return 0;
}
