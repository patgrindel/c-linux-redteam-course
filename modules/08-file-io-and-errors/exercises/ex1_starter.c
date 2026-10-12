/* ex1_starter.c - count the lines in a file. Fill in the TODOs.
 *
 * Build: gcc -std=c11 -Wall -Wextra ex1_starter.c -o ex1
 * Run:   ./ex1 <file>
 */
#include <stdio.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        perror(argv[1]);
        return 1;
    }

    long lines = 0;
    int c;
    /* TODO: read characters with fgetc(f) until EOF; count each '\n'. */
    (void) c;

    printf("%ld\n", lines);
    fclose(f);
    return 0;
}
