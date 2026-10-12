/* greet_main.c - uses the greet module.
 *
 * Build (compile both .c files, then let the linker combine them):
 *   gcc -std=c11 -Wall -Wextra greet_main.c greet.c -o greet
 * Run:
 *   ./greet
 *
 * This is "separate compilation": each .c becomes an object file, and the linker
 * resolves the call to greet() in this file against the definition in greet.c.
 * It is the same idea as Python's "import", but done at build time.
 */
#include "greet.h"       /* we only need the declaration to CALL greet */

int main(void)
{
    greet("world");
    greet("red team");
    return 0;
}
