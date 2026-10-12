/* checks_users.c -- enumerate local accounts from /etc/passwd.
 *
 * Why an assessor cares:
 *   - Any account with UID 0 is root-equivalent. A *second* UID 0 account is a
 *     classic backdoor and always worth flagging.
 *   - Accounts with a real login shell are interactive attack surface (and
 *     candidates for weak-password / reuse checks you'd do with authorization).
 *
 * Technique: read /etc/passwd line by line with getline(3) and split the
 * colon-separated fields in place (Module 08). Format per passwd(5):
 *   name:password:uid:gid:gecos:home:shell
 */
#define _GNU_SOURCE
#include "checks_users.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Split `line` into up to 7 passwd fields, in place (writes NULs over ':').
 * Returns the number of fields found. Missing trailing fields are left NULL. */
static int split_passwd(char *line, char *fields[7])
{
    for (int i = 0; i < 7; i++)
        fields[i] = NULL;

    int   n = 0;
    char *p = line;
    while (n < 7 && p != NULL) {
        fields[n++] = p;
        char *colon = strchr(p, ':');
        if (colon == NULL)
            break;
        *colon = '\0';
        p = colon + 1;
    }
    return n;
}

/* A shell counts as "interactive" unless it is one of the usual no-login
 * placeholders. */
static int is_login_shell(const char *shell)
{
    if (shell == NULL || shell[0] == '\0')
        return 0;
    const char *base = util_basename(shell);
    if (strcmp(base, "nologin") == 0 ||
        strcmp(base, "false") == 0 ||
        strcmp(base, "sync") == 0 ||
        strcmp(shell, "/dev/null") == 0)
        return 0;
    return 1;
}

void check_users(report_t *rep)
{
    report_section(rep, "Users & accounts");

    FILE *fp = fopen("/etc/passwd", "r");
    if (fp == NULL) {
        report_skip(rep, "could not open /etc/passwd");
        return;
    }

    char    *line = NULL;
    size_t   cap = 0;
    strvec_t interactive;
    strvec_t uid0;

    /* Initialise both separately: a short-circuited || would leave the second
     * vector uninitialised if the first init failed, and we later free both. */
    if (strvec_init(&interactive) != 0) {
        fclose(fp);
        report_skip(rep, "out of memory collecting accounts");
        return;
    }
    if (strvec_init(&uid0) != 0) {
        strvec_free(&interactive);
        fclose(fp);
        report_skip(rep, "out of memory collecting accounts");
        return;
    }

    while (getline(&line, &cap, fp) != -1) {
        util_rstrip(line);
        if (line[0] == '\0' || line[0] == '#')
            continue;

        char *fields[7];
        int   nf = split_passwd(line, fields);
        if (nf < 7)
            continue;   /* malformed line: skip rather than guess */

        const char *name  = fields[0];
        const char *uids  = fields[2];
        const char *shell = fields[6];

        long uid = strtol(uids, NULL, 10);
        if (uid == 0)
            strvec_push(&uid0, name);

        if (is_login_shell(shell)) {
            char entry[256];
            snprintf(entry, sizeof entry, "%s (uid %ld, %s)", name, uid, shell);
            strvec_push(&interactive, entry);
        }
    }

    free(line);
    fclose(fp);

    /* --- UID 0 accounts. --- */
    strvec_sort(&uid0);
    for (size_t i = 0; i < uid0.len; i++) {
        int extra = (strcmp(uid0.items[i], "root") != 0);
        report_item(rep, extra ? SEV_WARN : SEV_INFO,
                    "UID 0 account: %s", uid0.items[i]);
    }
    if (uid0.len > 1)
        report_detail(rep,
                      "More than one UID 0 account -- any non-root entry here "
                      "is root-equivalent and a likely backdoor.");

    /* --- Interactive accounts. --- */
    strvec_sort(&interactive);
    for (size_t i = 0; i < interactive.len; i++)
        report_item(rep, SEV_NOTE, "Login shell: %s", interactive.items[i]);
    if (interactive.len > 0)
        report_detail(rep,
                      "Interactive accounts are attack surface; note them for "
                      "authorized credential checks.");

    strvec_free(&interactive);
    strvec_free(&uid0);
}
