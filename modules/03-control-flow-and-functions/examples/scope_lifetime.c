/* scope_lifetime.c - where a name is visible, and how long its value lives.
 *
 * Build:  gcc -std=c11 -Wall -Wextra scope_lifetime.c -o scope_lifetime
 * Run:    ./scope_lifetime
 *
 * SCOPE is where a name can be used. LIFETIME is how long its storage exists.
 *   - An ordinary local variable is created on entry to its block and destroyed
 *     on exit: each call starts fresh.
 *   - A 'static' local keeps its storage for the whole program, so its value
 *     PERSISTS between calls (but it is still only visible inside the function).
 *   - A global (file-scope) variable is visible to the whole file and lives for
 *     the whole run.
 */
#include <stdio.h>

int g_config = 100;   /* global: file scope, lives the entire program */

/* Each call gets a new 'id' counter? No - 'static' makes it persist. */
static int next_id(void)
{
    static int id = 0;   /* initialized once; retains its value across calls */
    id++;
    return id;
}

int main(void)
{
    printf("g_config = %d\n", g_config);

    printf("next_id() -> %d\n", next_id());   /* 1 */
    printf("next_id() -> %d\n", next_id());   /* 2 */
    printf("next_id() -> %d\n", next_id());   /* 3 */

    /* Block scope: the inner x shadows the outer x only inside the braces. */
    int x = 1;
    {
        int x = 2;
        printf("inner block: x = %d\n", x);
    }
    printf("outer block: x = %d\n", x);
    return 0;
}
