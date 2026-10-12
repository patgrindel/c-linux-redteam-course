/* fork_exec_wait.c - run an external command: fork, exec in the child, wait in the parent.
 *
 * Build: gcc -std=c11 -Wall -Wextra fork_exec_wait.c -o fork_exec_wait
 * Run:   ./fork_exec_wait <cmd> [args...]     e.g. ./fork_exec_wait uname -sr
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <cmd> [args...]\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* child: argv+1 is already a NULL-terminated array of strings */
        execvp(argv[1], &argv[1]);
        perror("execvp");       /* only reached if exec failed */
        _exit(127);             /* 127 = "command not found", like a shell */
    }

    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return 1;
    }
    if (WIFEXITED(status)) {
        printf("[exited %d]\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("[killed by signal %d]\n", WTERMSIG(status));
    }
    return 0;
}
