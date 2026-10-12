/* byteorder.c - host vs. network byte order with htons/htonl.
 *
 * Build: gcc -std=c11 -Wall -Wextra byteorder.c -o byteorder
 * Run:   ./byteorder
 */
#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>

int main(void) {
    uint16_t port = 443;
    uint16_t net  = htons(port);
    printf("port 443: host=0x%04x network=0x%04x\n", port, net);

    uint32_t host_ip = 0x7f000001;        /* 127.0.0.1 as a 32-bit number */
    uint32_t net_ip  = htonl(host_ip);
    printf("ip 127.0.0.1: host=0x%08x network=0x%08x\n", host_ip, net_ip);
    return 0;
}
