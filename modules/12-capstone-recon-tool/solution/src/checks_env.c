/* checks_env.c -- inspect PATH and a couple of loader variables.
 *
 * Why an assessor cares: if PATH contains a writable or relative directory
 * *before* the system directories, an attacker can drop a malicious binary
 * that gets run in place of the real command -- "PATH hijacking". A program
 * that calls `system("somecmd")` while running as root and trusts PATH is the
 * classic victim. We also note LD_PRELOAD / LD_LIBRARY_PATH, which influence
 * the dynamic loader.
 *
 * Technique: read the process environment with getenv(3), split PATH by hand
 * (so we can catch empty components that strtok would hide), and stat each
 * directory (Module 08).
 */
#define _GNU_SOURCE
#include "checks_env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* Examine one PATH component. */
static void check_path_entry(report_t *rep, const char *entry)
{
    /* Empty component ("" between two colons, or a leading/trailing colon)
     * means "the current directory" -- as dangerous as a literal ".". */
    if (entry[0] == '\0') {
        report_item(rep, SEV_WARN,
                    "PATH has an empty element (means current directory)");
        return;
    }

    if (entry[0] != '/') {
        report_item(rep, SEV_WARN, "PATH has a relative element: '%s'", entry);
        report_detail(rep,
                      "Relative PATH entries are resolved from the current "
                      "directory -- trivially hijackable.");
        return;
    }

    struct stat st;
    if (stat(entry, &st) != 0) {
        report_skip(rep, "PATH element does not exist: %s", entry);
        return;
    }
    if (!S_ISDIR(st.st_mode)) {
        report_item(rep, SEV_NOTE, "PATH element is not a directory: %s", entry);
        return;
    }
    if (st.st_mode & S_IWOTH) {
        report_item(rep, SEV_WARN, "PATH element is world-writable: %s", entry);
        report_detail(rep,
                      "Anyone can plant a binary here that shadows a real "
                      "command.");
    }
}

void check_env(report_t *rep)
{
    report_section(rep, "Environment & PATH");

    const char *path = getenv("PATH");
    if (path == NULL) {
        report_skip(rep, "PATH is not set in this environment");
    } else {
        report_kv(rep, "PATH", path);

        /* Split on ':' by hand so empty components are visible. */
        char *copy = strdup(path);
        if (copy == NULL) {
            report_skip(rep, "out of memory parsing PATH");
        } else {
            char *start = copy;
            for (char *p = copy; ; p++) {
                if (*p == ':' || *p == '\0') {
                    int end = (*p == '\0');
                    *p = '\0';
                    check_path_entry(rep, start);
                    if (end)
                        break;
                    start = p + 1;
                }
            }
            free(copy);
        }
    }

    /* Loader variables worth a note if present. */
    static const char *const loader_vars[] = { "LD_PRELOAD", "LD_LIBRARY_PATH" };
    for (size_t i = 0; i < sizeof loader_vars / sizeof loader_vars[0]; i++) {
        const char *v = getenv(loader_vars[i]);
        if (v != NULL && v[0] != '\0')
            report_item(rep, SEV_NOTE, "%s is set: %s", loader_vars[i], v);
    }
}
