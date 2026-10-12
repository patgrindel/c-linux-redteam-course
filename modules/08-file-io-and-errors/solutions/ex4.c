/* ex4.c - robust file copy using raw descriptors.
 *
 * Approach: open src read-only, dst with O_CREAT|O_TRUNC (mode 0644); copy in
 * 4 KiB chunks; write_all loops over partial writes. Every call is checked and
 * descriptors are closed on every exit path.
 *
 * Build: gcc -std=c11 -Wall -Wextra ex4.c -o ex4
 * Run:   ./ex4 <src> <dst>
 */
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

static int write_all(int fd, const char *buf, size_t count) {
    size_t off = 0;
    while (off < count) {
        ssize_t n = write(fd, buf + off, count - off);
        if (n < 0) {
            return -1;
        }
        off += (size_t) n;
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <src> <dst>\n", argv[0]);
        return 1;
    }

    int src = open(argv[1], O_RDONLY);
    if (src < 0) {
        perror(argv[1]);
        return 1;
    }

    int dst = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst < 0) {
        perror(argv[2]);
        close(src);
        return 1;
    }

    char buf[4096];
    ssize_t n;
    int rc = 0;
    while ((n = read(src, buf, sizeof buf)) > 0) {
        if (write_all(dst, buf, (size_t) n) < 0) {
            perror(argv[2]);
            rc = 1;
            break;
        }
    }
    if (n < 0) {
        perror(argv[1]);
        rc = 1;
    }

    if (close(dst) < 0) {        /* a write error can surface at close */
        perror(argv[2]);
        rc = 1;
    }
    close(src);
    return rc;
}
