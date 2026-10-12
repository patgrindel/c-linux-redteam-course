/* bank.h - interface for a tiny balance module (static/extern demo).
 *
 * 'extern int g_balance;' is a DECLARATION: it says "a global int named
 * g_balance exists somewhere" without creating it. The single definition lives
 * in bank.c. Any .c that includes this header can then read/write that one
 * shared variable.
 */
#ifndef BANK_H
#define BANK_H

extern int g_balance;          /* declared here, defined once in bank.c */

void deposit(int amount);
void withdraw(int amount);

#endif /* BANK_H */
