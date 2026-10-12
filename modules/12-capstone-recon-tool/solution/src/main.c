/* main.c -- entry point: parse arguments, select checks, run them, summarise.
 *
 * AUTHORIZED USE ONLY. This tool inspects the LOCAL host it runs on and reports
 * its security posture. It does not exploit anything, does not change
 * privileges, and does not touch third-party systems. Run it only on a machine
 * you own or are explicitly permitted to assess.
 */
#define _GNU_SOURCE
#include "args.h"
#include "report.h"
#include "version.h"

#include "checks_system.h"
#include "checks_users.h"
#include "checks_suid.h"
#include "checks_worldwritable.h"
#include "checks_cron.h"
#include "checks_net.h"
#include "checks_env.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* The check registry. Adding a check is: write checks_X.c/.h, include it
 * above, and add one row here. `name` is what --only and --list use. */
typedef struct {
    const char *name;
    const char *desc;
    void      (*run)(report_t *rep);
} check_t;

static const check_t CHECKS[] = {
    { "system",        "OS, kernel and distribution identification", check_system },
    { "users",         "accounts, UID 0 and interactive shells",     check_users },
    { "suid",          "SUID/SGID binaries in common paths",         check_suid },
    { "worldwritable", "world-writable files and directories",       check_worldwritable },
    { "cron",          "scheduled jobs and writable cron files",     check_cron },
    { "net",           "listening TCP services",                     check_net },
    { "env",           "PATH and environment hygiene",               check_env },
};
#define N_CHECKS (sizeof CHECKS / sizeof CHECKS[0])

/* Is `name` present in the comma-separated list `csv`? */
static int name_in_csv(const char *csv, const char *name)
{
    size_t namelen = strlen(name);
    const char *p = csv;
    while (*p != '\0') {
        const char *comma = strchr(p, ',');
        size_t seglen = comma ? (size_t)(comma - p) : strlen(p);
        if (seglen == namelen && strncmp(p, name, namelen) == 0)
            return 1;
        if (comma == NULL)
            break;
        p = comma + 1;
    }
    return 0;
}

/* Warn about names in --only that don't match any known check. */
static void warn_unknown_only(const char *csv)
{
    const char *p = csv;
    while (*p != '\0') {
        const char *comma = strchr(p, ',');
        size_t seglen = comma ? (size_t)(comma - p) : strlen(p);

        if (seglen > 0) {
            int known = 0;
            for (size_t i = 0; i < N_CHECKS; i++)
                if (strlen(CHECKS[i].name) == seglen &&
                    strncmp(p, CHECKS[i].name, seglen) == 0) {
                    known = 1;
                    break;
                }
            if (!known)
                fprintf(stderr, "warning: unknown check in --only: '%.*s'\n",
                        (int)seglen, p);
        }
        if (comma == NULL)
            break;
        p = comma + 1;
    }
}

static void list_checks(FILE *out)
{
    fprintf(out, "Available checks:\n");
    for (size_t i = 0; i < N_CHECKS; i++)
        fprintf(out, "  %-14s %s\n", CHECKS[i].name, CHECKS[i].desc);
}

static void print_banner(report_t *rep)
{
    fprintf(rep->out,
            "%s %s -- local host posture assessment\n", RECON_PROG_NAME,
            RECON_VERSION);
    fprintf(rep->out,
            "AUTHORIZED USE ONLY: run only against a host you own or are "
            "permitted to assess.\n");
    fprintf(rep->out,
            "This tool reports posture; it does not exploit or change the "
            "system.\n");
}

int main(int argc, char **argv)
{
    options_t opts;
    if (args_parse(argc, argv, &opts) != 0)
        return 2;

    if (opts.show_help) {
        args_print_help(stdout, argv[0]);
        return 0;
    }
    if (opts.show_version) {
        args_print_version(stdout);
        return 0;
    }
    if (opts.list_checks) {
        list_checks(stdout);
        return 0;
    }

    /* Choose the output stream. */
    FILE *out = stdout;
    int   close_out = 0;
    if (opts.outfile != NULL) {
        out = fopen(opts.outfile, "w");
        if (out == NULL) {
            fprintf(stderr, "%s: cannot open '%s' for writing\n",
                    argv[0], opts.outfile);
            return 1;
        }
        close_out = 1;
    }

    /* Colour only when writing to a real terminal (and not disabled). */
    int use_color = !opts.no_color && isatty(fileno(out));

    report_t rep;
    report_init(&rep, out, use_color, opts.verbose);

    if (opts.only != NULL)
        warn_unknown_only(opts.only);

    print_banner(&rep);

    int ran = 0;
    for (size_t i = 0; i < N_CHECKS; i++) {
        if (opts.only != NULL && !name_in_csv(opts.only, CHECKS[i].name))
            continue;
        CHECKS[i].run(&rep);
        ran++;
    }

    if (ran == 0)
        fprintf(stderr, "no checks selected (see --list)\n");
    else
        report_summary(&rep);

    if (close_out)
        fclose(out);

    /* Exit code reflects "did we find a warning", not "did the tool fail" --
     * handy in scripts. 0 = clean-ish, 1 = at least one warning. */
    return (rep.n_warn > 0) ? 1 : 0;
}
