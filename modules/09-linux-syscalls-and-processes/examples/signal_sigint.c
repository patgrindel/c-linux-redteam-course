/* signal_sigint.c - catch Ctrl-C (SIGINT) with sigaction and exit cleanly.
 *
 * The handler only sets a volatile sig_atomic_t flag; the real work (printing,
 * cleanup) happens back in main(). That is the safe handler pattern.
 *
 * Build: gcc -std=c11 -Wall -Wextra signal_sigint.c -o signal_sigint
 * Run:   ./signal_sigint      (press Ctrl-C to stop)
 */
#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdio.h>

static volatile sig_atomic_t stop = 0;

static void on_sigint(int sig) {
    (void) sig;
    stop = 1;
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

    printf("working... press Ctrl-C to stop\n");
    unsigned long i = 0;
    while (!stop) {
        i++;
        if (i % 100000000UL == 0) {
            printf("still working (%lu)\n", i);
            fflush(stdout);
        }
    }

    printf("\ncaught SIGINT, cleaning up, bye\n");
    return 0;
}
