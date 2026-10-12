/* bank_main.c - uses the bank module and the shared extern global.
 *
 * Build:  gcc -std=c11 -Wall -Wextra bank_main.c bank.c -o bank
 * Run:    ./bank
 */
#include <stdio.h>
#include "bank.h"

int main(void)
{
    deposit(100);
    withdraw(30);
    withdraw(1000);   /* denied */

    /* We can read g_balance directly because bank.h declared it extern. */
    printf("final balance (via extern global): %d\n", g_balance);
    return 0;
}
