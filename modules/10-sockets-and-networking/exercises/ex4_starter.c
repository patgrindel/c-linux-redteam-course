/* ex4_starter.c - port scanner with a configurable timeout + summary line.
 * Copy connect_timeout() from examples/port_scan.c and fill in the TODOs.
 *
 * Build: gcc -std=c11 -Wall -Wextra ex4_starter.c -o ex4
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

/* TODO: paste connect_timeout(host, port, timeout_ms) from examples/port_scan.c */
static int connect_timeout(const char *host, const char *port, int timeout_ms);

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

    /* TODO: for each port, connect_timeout; collect open ports into a dynamic
       array (malloc/realloc); at the end print "open: p1, p2, ...". */
    (void) connect_timeout;
    return 0;
}
