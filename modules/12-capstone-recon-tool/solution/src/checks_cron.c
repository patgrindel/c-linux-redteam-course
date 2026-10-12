/* checks_cron.c -- enumerate scheduled jobs and flag writable cron files.
 *
 * Why an assessor cares: cron jobs frequently run as root. If a job's script
 * (or the crontab file itself) is writable by a lower-privileged user, that is
 * a direct path to code execution as root -- a textbook privesc. We list the
 * active entries and warn on any cron file that is group/world writable.
 *
 * Technique: read crontab-format files line by line (Module 08) and walk the
 * cron directories with opendir/readdir (Module 09-adjacent directory I/O).
 */
#define _GNU_SOURCE
#include "checks_cron.h"
#include "util.h"

#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

/* Warn if a cron file is writable by group or other. */
static void flag_if_writable(report_t *rep, const char *path)
{
    struct stat st;
    if (stat(path, &st) != 0)
        return;
    if (st.st_mode & (S_IWGRP | S_IWOTH)) {
        char modestr[11];
        util_mode_str(st.st_mode, modestr);
        report_item(rep, SEV_WARN,
                    "cron file is non-root-writable: %s %s", modestr, path);
        report_detail(rep,
                      "If this runs as root, a writable entry = root code "
                      "execution.");
    }
}

/* Print the active (non-comment, non-blank, non-assignment) lines of a
 * crontab-format file, up to a small cap so output stays readable. */
static void dump_crontab(report_t *rep, const char *path)
{
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        report_skip(rep, "could not read %s", path);
        return;
    }

    char line[1024];
    int  shown = 0;
    while (fgets(line, sizeof line, fp) != NULL && shown < 20) {
        util_rstrip(line);

        /* Skip blanks, comments, and VAR=value assignment lines. */
        char *p = line;
        while (*p == ' ' || *p == '\t')
            p++;
        if (*p == '\0' || *p == '#')
            continue;
        if (strchr(p, ' ') == NULL && strchr(p, '=') != NULL)
            continue;

        report_item(rep, SEV_NOTE, "%s: %s", util_basename(path), p);
        shown++;
    }
    fclose(fp);
}

/* List the job scripts in a cron.{hourly,daily,...} directory and flag any
 * that are non-root writable. */
static void scan_cron_dir(report_t *rep, const char *dir)
{
    DIR *d = opendir(dir);
    if (d == NULL)
        return;     /* absent dir is normal; stay quiet unless verbose */

    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        if (ent->d_name[0] == '.')
            continue;

        char path[PATH_MAX];
        int n = snprintf(path, sizeof path, "%s/%s", dir, ent->d_name);
        if (n < 0 || (size_t)n >= sizeof path)
            continue;

        struct stat st;
        if (stat(path, &st) != 0 || !S_ISREG(st.st_mode))
            continue;

        report_item(rep, SEV_INFO, "%s/%s", util_basename(dir), ent->d_name);
        flag_if_writable(rep, path);
    }
    closedir(d);
}

void check_cron(report_t *rep)
{
    report_section(rep, "Scheduled jobs (cron)");

    /* The system crontab. */
    struct stat st;
    if (stat("/etc/crontab", &st) == 0) {
        dump_crontab(rep, "/etc/crontab");
        flag_if_writable(rep, "/etc/crontab");
    } else {
        report_skip(rep, "no /etc/crontab");
    }

    /* Drop-in crontabs in /etc/cron.d. */
    DIR *cd = opendir("/etc/cron.d");
    if (cd != NULL) {
        struct dirent *ent;
        while ((ent = readdir(cd)) != NULL) {
            if (ent->d_name[0] == '.')
                continue;
            char path[PATH_MAX];
            int n = snprintf(path, sizeof path, "/etc/cron.d/%s", ent->d_name);
            if (n < 0 || (size_t)n >= sizeof path)
                continue;
            dump_crontab(rep, path);
            flag_if_writable(rep, path);
        }
        closedir(cd);
    }

    /* The periodic run-parts directories. */
    static const char *const periodic[] = {
        "/etc/cron.hourly", "/etc/cron.daily",
        "/etc/cron.weekly", "/etc/cron.monthly",
    };
    for (size_t i = 0; i < sizeof periodic / sizeof periodic[0]; i++)
        scan_cron_dir(rep, periodic[i]);
}
