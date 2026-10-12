/* checks_net.c -- which TCP services are listening, and on which interface.
 *
 * Why an assessor cares: a service bound to 127.0.0.1 is reachable only
 * locally; the same service bound to 0.0.0.0 is remote attack surface. Knowing
 * what listens -- and whether it's exposed off-box -- shapes the whole
 * engagement.
 *
 * Two techniques, deliberately, to show both halves of the course:
 *   1. Parse /proc/net/tcp and /proc/net/tcp6 (Module 08/09). This is the
 *      authoritative list of listening sockets, including ones bound to
 *      addresses we can't connect to.
 *   2. A tiny non-blocking TCP connect probe against a few common ports on
 *      127.0.0.1 (Module 10). This confirms what actually accepts a
 *      connection and demonstrates the sockets API. It only ever touches
 *      localhost.
 */
#define _GNU_SOURCE
#include "checks_net.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#define TCP_STATE_LISTEN 0x0A

/* Best-effort friendly name for a well-known port. */
static const char *port_name(unsigned port)
{
    switch (port) {
    case 21:   return "ftp";
    case 22:   return "ssh";
    case 23:   return "telnet";
    case 25:   return "smtp";
    case 53:   return "dns";
    case 80:   return "http";
    case 110:  return "pop3";
    case 139:  return "netbios";
    case 143:  return "imap";
    case 443:  return "https";
    case 445:  return "smb";
    case 3306: return "mysql";
    case 3389: return "rdp";
    case 5432: return "postgres";
    case 5900: return "vnc";
    case 6379: return "redis";
    case 8080: return "http-alt";
    case 8443: return "https-alt";
    default:   return "unknown";
    }
}

/* Parse one /proc/net/tcp{,6} file and report LISTEN sockets. */
static void parse_proc_net(report_t *rep, const char *path, int v6)
{
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        report_skip(rep, "could not read %s", path);
        return;
    }

    char line[512];
    if (fgets(line, sizeof line, fp) == NULL) {   /* discard the header row */
        fclose(fp);
        return;
    }

    while (fgets(line, sizeof line, fp) != NULL) {
        char     localaddr[64];
        unsigned lport = 0;
        unsigned state = 0;

        /* sl: LOCAL:PORT REMOTE:PORT STATE ...  (we skip remote and the rest) */
        if (sscanf(line, "%*d: %63[0-9A-Fa-f]:%x %*[0-9A-Fa-f]:%*x %x",
                   localaddr, &lport, &state) != 3)
            continue;
        if (state != TCP_STATE_LISTEN)
            continue;

        if (!v6) {
            /* IPv4 address is a little-endian hex uint32. */
            unsigned long a = strtoul(localaddr, NULL, 16);
            unsigned b0 = a & 0xff, b1 = (a >> 8) & 0xff;
            unsigned b2 = (a >> 16) & 0xff, b3 = (a >> 24) & 0xff;

            severity_t  sev;
            const char *scope;
            if (a == 0) {            sev = SEV_WARN; scope = "all interfaces"; }
            else if (b0 == 127) {    sev = SEV_INFO; scope = "loopback"; }
            else {                   sev = SEV_NOTE; scope = "specific iface"; }

            report_item(rep, sev, "tcp  %u.%u.%u.%u:%u  %-9s [%s]",
                        b0, b1, b2, b3, lport, port_name(lport), scope);
        } else {
            severity_t  sev;
            const char *disp;
            if (strcmp(localaddr, "00000000000000000000000000000000") == 0) {
                sev = SEV_WARN; disp = "[::]";            /* any */
            } else if (strcmp(localaddr,
                              "00000000000000000000000001000000") == 0) {
                sev = SEV_INFO; disp = "[::1]";           /* loopback */
            } else {
                sev = SEV_NOTE; disp = "[ipv6]";
            }
            report_item(rep, sev, "tcp6 %s:%u  %-9s",
                        disp, lport, port_name(lport));
        }
    }

    fclose(fp);
}

/* Non-blocking TCP connect to 127.0.0.1:port with a short timeout.
 * Returns 1 if the port accepts a connection, 0 otherwise. */
static int probe_localhost(unsigned port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        return 0;

    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        close(fd);
        return 0;
    }

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET;
    sa.sin_port = htons((uint16_t)port);
    sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);   /* always 127.0.0.1 */

    int open_port = 0;
    int rc = connect(fd, (struct sockaddr *)&sa, sizeof sa);
    if (rc == 0) {
        open_port = 1;                              /* connected immediately */
    } else if (errno == EINPROGRESS) {
        fd_set wfds;
        FD_ZERO(&wfds);
        FD_SET(fd, &wfds);
        struct timeval tv = { .tv_sec = 0, .tv_usec = 150000 };  /* 150 ms */

        if (select(fd + 1, NULL, &wfds, NULL, &tv) > 0) {
            int       soerr = 0;
            socklen_t len = sizeof soerr;
            if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &soerr, &len) == 0 &&
                soerr == 0)
                open_port = 1;
        }
    }

    close(fd);
    return open_port;
}

void check_net(report_t *rep)
{
    report_section(rep, "Listening TCP services");

    parse_proc_net(rep, "/proc/net/tcp", 0);
    parse_proc_net(rep, "/proc/net/tcp6", 1);

    report_detail(rep,
                  "A service on all interfaces is reachable off-box; the same "
                  "service on loopback is local-only.");

    /* Sockets-based confirmation against localhost only. */
    static const unsigned common[] = {
        21, 22, 23, 25, 80, 110, 143, 443, 445,
        3306, 3389, 5432, 5900, 6379, 8080, 8443,
    };
    int any = 0;
    for (size_t i = 0; i < sizeof common / sizeof common[0]; i++) {
        if (probe_localhost(common[i])) {
            report_item(rep, SEV_NOTE, "connect ok: 127.0.0.1:%u (%s)",
                        common[i], port_name(common[i]));
            any = 1;
        }
    }
    if (!any)
        report_item(rep, SEV_INFO,
                    "no common ports accepted a localhost connection");
}
