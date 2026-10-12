/* sigchld_reap.c - reap children from a SIGCHLD handler so none become zombies.
 *
 * The handler saves/restores errno (waitpid can change it) and reaps every
 * finished child with WNOHANG so it never blocks.
 *
 * Build: gcc -std=c11 -Wall -Wextra sigchld_reap.c -o sigchld_reap
 * Run:   ./sigchld_reap
 */
#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

static void reap(int sig) {
    (void) sig;
    int saved = errno;                        /* handler must not clobber errno */
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        /* reap all finished children without blocking */
    }
    errno = saved;
}

int main(void) {
    struct sigaction sa = {0};
    sa.sa_handler = reap;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;   /* retry interrupted calls; exit-only */
    if (sigaction(SIGCHLD, &sa, NULL) < 0) {
        perror("sigaction");
        return 1;
    }

    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            printf("  child %d (pid %d) running\n", i, (int) getpid());
            _exit(0);
        }
    }

    for (int t = 0; t < 5; t++) {
        printf("parent tick %d\n", t);
        sleep(1);
    }
    printf("parent done, no zombies left behind\n");
    return 0;
}
