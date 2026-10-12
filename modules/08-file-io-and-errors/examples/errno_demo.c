/* errno_demo.c - how a failed call reports why, via errno/perror/strerror.
 *
 * Build: gcc -std=c11 -Wall -Wextra errno_demo.c -o errno_demo
 * Run:   ./errno_demo
 */
#include <errno.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    FILE *f = fopen("/root/secret.txt", "r");   /* usually unreadable to you */
    if (f == NULL) {
        /* Two equivalent ways to report the same failure: */
        perror("open /root/secret.txt");
        fprintf(stderr, "errno=%d: %s\n", errno, strerror(errno));
        return 1;
    }
    fclose(f);
    return 0;
}
