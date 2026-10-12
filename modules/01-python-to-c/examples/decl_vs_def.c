/* decl_vs_def.c - a declaration promises; a definition delivers.
 *
 * Build:  gcc -std=c11 -Wall -Wextra decl_vs_def.c -o decl_vs_def
 * Run:    ./decl_vs_def
 *
 * The compiler reads top to bottom. To call square() before its full body
 * appears, we first DECLARE it (a prototype: name, return type, parameters).
 * The DEFINITION (the body) comes later. The linker connects the two.
 */
#include <stdio.h>

int square(int n);   /* declaration (prototype): "square exists, trust me" */

int main(void)
{
    int result = square(9);   /* legal: the compiler already knows square's shape */
    printf("9 squared is %d\n", result);
    return 0;
}

int square(int n)    /* definition: the actual code */
{
    return n * n;
}
