/* exit_code.c - the value returned from main becomes the process exit code.
 *
 * Build:  gcc -std=c11 -Wall -Wextra exit_code.c -o exit_code
 * Run:    ./exit_code ; echo "exit code was $?"
 *
 * The shell variable $? holds the exit code of the last command. By convention
 * 0 means success and any non-zero value signals a specific kind of failure.
 */
#include <stdio.h>

int main(void)
{
    printf("Doing some work, then failing on purpose.\n");
    return 3;   /* pretend "3" is our code for a particular error */
}
