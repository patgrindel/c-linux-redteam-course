/* args.h -- command-line parsing for reconc. */
#ifndef ARGS_H
#define ARGS_H

#include <stdio.h>

typedef struct {
    int         show_help;      /* -h / --help */
    int         show_version;   /* -V / --version */
    int         list_checks;    /* --list */
    int         no_color;       /* --no-color */
    int         verbose;        /* -v / --verbose */
    const char *only;           /* --only=a,b,c  (NULL means "run all") */
    const char *outfile;        /* -o FILE / --output FILE (NULL means stdout) */
} options_t;

/* Parse argv into *opts. Returns 0 on success, or -1 on a usage error
 * (a message is printed to stderr in that case). Defaults are filled in
 * for every field, so the caller may use *opts even after an error if it
 * only wants to print help. */
int args_parse(int argc, char **argv, options_t *opts);

void args_print_help(FILE *out, const char *prog);
void args_print_version(FILE *out);

#endif /* ARGS_H */
