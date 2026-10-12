/* bank.c - implementation; shows 'static' (internal linkage) vs a shared global.
 *
 * Built together with bank_main.c:
 *   gcc -std=c11 -Wall -Wextra bank_main.c bank.c -o bank
 */
#include <stdio.h>
#include "bank.h"

int g_balance = 0;   /* THE definition of the global declared in bank.h */

/* 'static' on a function gives it internal linkage: it is private to this file.
 * Other .c files cannot see or call log_change - good for hiding helpers. */
static void log_change(const char *action, int amount)
{
    printf("  [%s %d] balance now %d\n", action, amount, g_balance);
}

void deposit(int amount)
{
    g_balance += amount;
    log_change("deposit", amount);
}

void withdraw(int amount)
{
    if (amount > g_balance) {
        printf("  [withdraw %d] denied, only %d available\n", amount, g_balance);
        return;
    }
    g_balance -= amount;
    log_change("withdraw", amount);
}
