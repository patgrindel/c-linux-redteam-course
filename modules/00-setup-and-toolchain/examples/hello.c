/* hello.c - the smallest useful C program.
 *
 * Build:  gcc -std=c11 -Wall -Wextra hello.c -o hello
 * Run:    ./hello
 */
#include <stdio.h>   /* declares printf */

int main(void)
{
    printf("Hello, C!\n");
    return 0;        /* 0 means "success" to the shell */
}
