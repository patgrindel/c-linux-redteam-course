/* fd_cat.c - a `cat` built on raw file descriptors (open/read/write/close).
 *
 * Demonstrates the partial-write loop: write() may transfer fewer bytes than
 * requested, so we loop until the whole buffer is out.
 *
 * Build: gcc -std=c11 -Wall -Wextra fd_cat.c -o fd_cat
 * Run:   ./fd_cat <file>
 */
#include <fcntl.h>      /* open, O_RDONLY */
#include <unistd.h>     /* read, write, close, STDOUT_FILENO */
#include <stdio.h>

/* Write the whole buffer, looping because write() may do it in pieces. */
static int write_all(int fd, const char *buf, size_t count) {
    size_t off = 0;
    while (off < count) {
        ssize_t n = write(fd, buf + off, count - off);
        if (n < 0) {
            return -1;          /* caller inspects errno */
        }
        off += (size_t) n;
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror(argv[1]);
        return 1;
    }

    char buf[4096];
    ssize_t n;
    while ((n = read(fd, buf, sizeof buf)) > 0) {
        if (write_all(STDOUT_FILENO, buf, (size_t) n) < 0) {
            perror("write");
            close(fd);
            return 1;
        }
    }
    if (n < 0) {                 /* read error, not EOF */
        perror(argv[1]);
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}
