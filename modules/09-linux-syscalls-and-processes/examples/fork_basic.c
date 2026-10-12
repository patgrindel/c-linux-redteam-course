/* fork_basic.c - fork() returns twice; parent waits for the child's status.
 *
 * _POSIX_C_SOURCE exposes fork/getpid/waitpid and the WIF* macros under -std=c11.
 *
 * Build: gcc -std=c11 -Wall -Wextra fork_basic.c -o fork_basic
 * Run:   ./fork_basic
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    printf("before fork: pid=%d\n", (int) getpid());

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* child */
        printf("  child:  my pid=%d, parent=%d\n", (int) getpid(), (int) getppid());
        return 42;                  /* becomes the child's exit status */
    }

    /* parent */
    printf("parent: forked child pid=%d\n", (int) pid);

    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return 1;
    }
    if (WIFEXITED(status)) {
        printf("parent: child exited with %d\n", WEXITSTATUS(status));
    }
    return 0;
}
