/* ex1_starter.c - implement mysystem(): run a command via /bin/sh -c.
 *
 * Build: gcc -std=c11 -Wall -Wextra ex1_starter.c -o ex1
 * Run:   ./ex1 '<command>'
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

static int mysystem(const char *cmd) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }
    if (pid == 0) {
        /* TODO: execl("/bin/sh", "sh", "-c", cmd, (char *)NULL);
                 on failure: perror + _exit(127) */
        (void) cmd;
        _exit(127);
    }
    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return -1;
    }
    /* TODO: return WEXITSTATUS(status) when WIFEXITED(status). */
    return -1;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s '<command>'\n", argv[0]);
        return 1;
    }
    int rc = mysystem(argv[1]);
    return (rc < 0) ? 1 : rc;
}
