/* run_capture.c - run a command and capture its stdout through a pipe.
 *
 * Pattern: pipe() -> fork() -> child dup2's the pipe write end onto stdout and
 * execs; parent closes the write end (so it sees EOF) and reads the output into
 * a growable buffer. This is how the capstone shells out to tools and parses them.
 *
 * Build: gcc -std=c11 -Wall -Wextra run_capture.c -o run_capture
 * Run:   ./run_capture
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

/* Run cmd[], capture stdout into *out (malloc'd, NUL-terminated).
 * Returns the child's exit status, or -1 on setup failure. Caller frees *out. */
static int run_capture(char *const cmd[], char **out) {
    int fds[2];
    if (pipe(fds) < 0) {
        perror("pipe");
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        close(fds[0]);
        close(fds[1]);
        return -1;
    }

    if (pid == 0) {
        /* child: send stdout into the pipe, then exec */
        close(fds[0]);                  /* child doesn't read */
        if (dup2(fds[1], STDOUT_FILENO) < 0) {
            perror("dup2");
            _exit(127);
        }
        close(fds[1]);
        execvp(cmd[0], cmd);
        perror("execvp");
        _exit(127);
    }

    /* parent: read everything from the pipe */
    close(fds[1]);                      /* must close write end to see EOF */

    size_t cap = 4096, len = 0;
    char *buf = malloc(cap);
    if (buf == NULL) {
        perror("malloc");
        close(fds[0]);
        return -1;
    }

    ssize_t n;
    while ((n = read(fds[0], buf + len, cap - len - 1)) > 0) {
        len += (size_t) n;
        if (len + 1 >= cap) {           /* grow, keep room for the NUL */
            cap *= 2;
            char *bigger = realloc(buf, cap);
            if (bigger == NULL) {
                perror("realloc");
                free(buf);
                close(fds[0]);
                return -1;
            }
            buf = bigger;
        }
    }
    buf[len] = '\0';
    close(fds[0]);

    int status;
    waitpid(pid, &status, 0);
    *out = buf;
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}

int main(void) {
    char *const cmd[] = { "id", "-un", NULL };   /* print the current username */
    char *output = NULL;

    int rc = run_capture(cmd, &output);
    if (rc < 0) {
        return 1;
    }
    printf("exit=%d, captured: %s", rc, output);   /* output already ends in '\n' */
    free(output);
    return 0;
}
