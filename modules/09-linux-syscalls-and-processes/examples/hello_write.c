/* hello_write.c - minimal program to watch under strace.
 *
 * Uses the write(2) syscall directly (no stdio buffering) so the strace output
 * shows a single clean write(1, "hi\n", 3) line.
 *
 * Build: gcc -std=c11 -Wall -Wextra hello_write.c -o hello_write
 * Run:   strace -f ./hello_write
 */
#include <string.h>
#include <unistd.h>

int main(void) {
    const char *msg = "hi\n";
    if (write(STDOUT_FILENO, msg, strlen(msg)) < 0) {
        return 1;
    }
    return 0;
}
