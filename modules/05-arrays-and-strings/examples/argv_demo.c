#include <stdio.h>
#include <string.h>

/* argc is the argument count; argv is an array of C strings. argv[0] is the
   program name, argv[1..argc-1] are the arguments, and argv[argc] is NULL.
   This is a tiny flag parser like the ones you will write for the capstone. */
int main(int argc, char **argv) {
    printf("program name: %s\n", argv[0]);
    printf("got %d argument(s)\n", argc - 1);

    int verbose = 0;
    const char *target = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            target = argv[++i];        /* consume the next arg as -t's value */
        } else {
            printf("unknown arg: %s\n", argv[i]);
        }
    }

    printf("verbose = %s\n", verbose ? "on" : "off");
    printf("target  = %s\n", target ? target : "(none)");
    return 0;
}
