/* greet.c - the IMPLEMENTATION of the greet module.
 *
 * This file provides the definition promised by greet.h. It includes its own
 * header so the compiler can check the definition matches the declaration.
 */
#include <stdio.h>
#include "greet.h"

void greet(const char *name)
{
    printf("Hello, %s! Welcome to C.\n", name);
}
