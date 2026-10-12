/* ex1_main.c - driver for the greet exercise. Write a Makefile that builds this
 * plus greeter.c into a program called `greet`.
 */
#include "greeter.h"

int main(int argc, char **argv) {
    const char *name = (argc > 1) ? argv[1] : "world";
    greet(name);
    return 0;
}
