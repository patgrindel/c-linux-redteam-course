/* args.c -- a small, hand-rolled argument parser.
 *
 * We parse by hand (rather than getopt_long) so the code stays self-contained
 * and easy to read against what Module 05 taught about argv. The grammar is
 * deliberately tiny.
 */
#define _GNU_SOURCE
#include "args.h"
#include "version.h"

#include <string.h>

void args_print_version(FILE *out)
{
    fprintf(out, "%s %s\n", RECON_PROG_NAME, RECON_VERSION);
}

void args_print_help(FILE *out, const char *prog)
{
    fprintf(out,
"%s %s -- local Linux host posture assessment\n"
"\n"
"AUTHORIZED USE ONLY. Run this only on a host you own or are explicitly\n"
"permitted to assess. It reports the local security posture; it does not\n"
"exploit anything, change privileges, or touch third-party systems.\n"
"\n"
"Usage: %s [options]\n"
"\n"
"Options:\n"
"  -h, --help            Show this help and exit.\n"
"  -V, --version         Show version and exit.\n"
"      --list            List the available checks and exit.\n"
"      --only=LIST       Run only these checks (comma-separated names).\n"
"  -o, --output FILE     Write the report to FILE instead of stdout.\n"
"  -v, --verbose         Show extra detail and skipped-scan notices.\n"
"      --no-color        Disable ANSI colour even on a terminal.\n"
"\n"
"Examples:\n"
"  %s                      # run every check against localhost\n"
"  %s --only=suid,users    # just the SUID and account checks\n"
"  %s -v -o report.txt     # verbose, saved to a file\n"
"\n"
"Each finding is tagged [*] info, [+] note (worth a look), or [!] warning\n"
"(a common weakness). See the module README for what each check means.\n",
            RECON_PROG_NAME, RECON_VERSION, prog, prog, prog, prog);
}

/* Does argument `arg` equal "--name" or begin with "--name="?  If it begins
 * with "--name=", return a pointer to the value after '='; if it equals
 * "--name" exactly, return the empty string; otherwise return NULL. */
static const char *match_long_eq(const char *arg, const char *name)
{
    size_t n = strlen(name);
    if (strncmp(arg, "--", 2) != 0)
        return NULL;
    if (strncmp(arg + 2, name, n) != 0)
        return NULL;
    if (arg[2 + n] == '\0')
        return arg + 2 + n;     /* exact match -> "" */
    if (arg[2 + n] == '=')
        return arg + 2 + n + 1; /* value after '=' */
    return NULL;
}

int args_parse(int argc, char **argv, options_t *opts)
{
    /* Defaults. */
    opts->show_help = 0;
    opts->show_version = 0;
    opts->list_checks = 0;
    opts->no_color = 0;
    opts->verbose = 0;
    opts->only = NULL;
    opts->outfile = NULL;

    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
        const char *val;

        if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            opts->show_help = 1;
        } else if (strcmp(arg, "-V") == 0 || strcmp(arg, "--version") == 0) {
            opts->show_version = 1;
        } else if (strcmp(arg, "--list") == 0) {
            opts->list_checks = 1;
        } else if (strcmp(arg, "-v") == 0 || strcmp(arg, "--verbose") == 0) {
            opts->verbose = 1;
        } else if (strcmp(arg, "--no-color") == 0 ||
                   strcmp(arg, "--no-colour") == 0) {
            opts->no_color = 1;
        } else if ((val = match_long_eq(arg, "only")) != NULL) {
            if (val[0] == '\0') {
                fprintf(stderr, "%s: --only needs a value, e.g. --only=suid\n",
                        argv[0]);
                return -1;
            }
            opts->only = val;
        } else if (strcmp(arg, "-o") == 0 || strcmp(arg, "--output") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "%s: %s needs a filename\n", argv[0], arg);
                return -1;
            }
            opts->outfile = argv[++i];
        } else if ((val = match_long_eq(arg, "output")) != NULL &&
                   val[0] != '\0') {
            opts->outfile = val;            /* --output=FILE form */
        } else {
            fprintf(stderr, "%s: unknown option '%s' (try --help)\n",
                    argv[0], arg);
            return -1;
        }
    }

    return 0;
}
