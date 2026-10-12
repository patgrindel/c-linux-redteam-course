/* ex2.c - return a chosen exit code.
 *
 * Approach: whatever int main returns becomes the process exit status visible in
 * $?. We return 42 directly. Note that $? is only one byte (0-255): returning 300
 * would show up as 300 % 256 == 44, which is why small, deliberate codes are best.
 */
#include <stdio.h>

int main(void)
{
    printf("Exiting with a chosen code.\n");
    return 42;
}
