/* call_stack.c - watch the call stack grow and unwind via recursion.
 *
 * Build:  gcc -std=c11 -Wall -Wextra call_stack.c -o call_stack
 * Run:    ./call_stack
 *
 * Every function call gets its own "stack frame" holding its parameters and
 * locals. A recursive call stacks a new frame on top; when it returns, that
 * frame is popped and we resume where we left off. The indentation below is the
 * depth of the stack. (Each frame here has its own 'n' - pass-by-value again.)
 */
#include <stdio.h>

static long factorial(int n, int depth)
{
    printf("%*scall factorial(%d)\n", depth * 2, "", n);

    long result;
    if (n <= 1) {
        result = 1;                                   /* base case: stop recursing */
    } else {
        result = (long)n * factorial(n - 1, depth + 1);  /* one frame deeper */
    }

    printf("%*sreturn %ld\n", depth * 2, "", result);
    return result;
}

int main(void)
{
    printf("computing 4!\n");
    printf("4! = %ld\n", factorial(4, 0));
    return 0;
}
