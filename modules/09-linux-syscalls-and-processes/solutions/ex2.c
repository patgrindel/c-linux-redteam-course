/* ex2.c - capture `uname -a` output via a pipe and report its byte length.
 *
 * Approach: the pipe/fork/dup2/exec capture pattern; the parent closes the write
 * end so read() sees EOF, accumulates into a growable buffer, then prints the
 * text and strlen.
 *
 * Build: gcc -std=c11 -Wall -Wextra ex2.c -o ex2
 * Run:   ./ex2
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static char *capture(char *const cmd[]) {
    int fds[2];
    if (pipe(fds) < 0) {
        perror("pipe");
        return NULL;
    }
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        close(fds[0]);
        close(fds[1]);
        return NULL;
    }
    if (pid == 0) {
        close(fds[0]);
        if (dup2(fds[1], STDOUT_FILENO) < 0) {
            perror("dup2");
            _exit(127);
        }
        close(fds[1]);
        execvp(cmd[0], cmd);
        perror("execvp");
        _exit(127);
    }

    close(fds[1]);
    size_t cap = 4096, len = 0;
    char *buf = malloc(cap);
    if (buf == NULL) {
        perror("malloc");
        close(fds[0]);
        return NULL;
    }
    ssize_t n;
    while ((n = read(fds[0], buf + len, cap - len - 1)) > 0) {
        len += (size_t) n;
        if (len + 1 >= cap) {
            cap *= 2;
            char *bigger = realloc(buf, cap);
            if (bigger == NULL) {
                perror("realloc");
                free(buf);
                close(fds[0]);
                return NULL;
            }
            buf = bigger;
        }
    }
    buf[len] = '\0';
    close(fds[0]);
    waitpid(pid, NULL, 0);
    return buf;
}

int main(void) {
    char *const cmd[] = { "uname", "-a", NULL };
    char *out = capture(cmd);
    if (out == NULL) {
        return 1;
    }
    fputs(out, stdout);                      /* already ends in '\n' */
    printf("captured %zu bytes\n", strlen(out));
    free(out);
    return 0;
}
