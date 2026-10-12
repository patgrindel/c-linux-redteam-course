/* ex3.c - two-strike Ctrl-C: warn on the first SIGINT, exit on the second.
 *
 * Approach: the handler only increments a volatile sig_atomic_t and write()s a
 * fixed warning string (write is async-signal-safe; printf is not). main() loops
 * until the counter reaches 2.
 *
 * Build: gcc -std=c11 -Wall -Wextra ex3.c -o ex3
 * Run:   ./ex3     (Ctrl-C twice)
 */
#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static volatile sig_atomic_t hits = 0;

static void on_sigint(int sig) {
    (void) sig;
    hits++;
    if (hits == 1) {
        static const char msg[] = "\npress Ctrl-C again to quit\n";
        /* ignore the return value deliberately in a handler */
        ssize_t w = write(STDOUT_FILENO, msg, sizeof msg - 1);
        (void) w;
    }
}

int main(void) {
    struct sigaction sa = {0};
    sa.sa_handler = on_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("sigaction");
        return 1;
    }

    printf("running; Ctrl-C twice to quit\n");
    while (hits < 2) {
        pause();            /* sleep until any signal arrives */
    }
    printf("exiting\n");
    return 0;
}
