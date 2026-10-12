/* checks_system.c -- identify the OS, kernel and distribution.
 *
 * Why an assessor cares: the kernel release and distro version are the first
 * thing you map against known local-privilege-escalation CVEs. Flagging an old
 * kernel on a box you're assessing is exactly the kind of note you want.
 *
 * Techniques used:
 *   - uname(2)            : kernel/arch identity straight from the syscall.
 *   - /proc/version       : the exact kernel build string (Module 09).
 *   - /etc/os-release     : distro name, parsed as KEY=VALUE text (Module 08).
 */
#define _GNU_SOURCE
#include "checks_system.h"
#include "util.h"

#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>

/* Pull a single KEY=VALUE out of /etc/os-release, stripping surrounding
 * quotes. Returns 0 and fills `out` on success, -1 otherwise. */
static int os_release_value(const char *key, char *out, size_t outlen)
{
    FILE *fp = fopen("/etc/os-release", "r");
    if (fp == NULL)
        return -1;

    char   line[512];
    size_t keylen = strlen(key);
    int    found = -1;

    while (fgets(line, sizeof line, fp) != NULL) {
        if (strncmp(line, key, keylen) != 0 || line[keylen] != '=')
            continue;

        char *val = line + keylen + 1;
        util_rstrip(val);

        /* Drop one pair of surrounding double quotes if present. */
        size_t vlen = strlen(val);
        if (vlen >= 2 && val[0] == '"' && val[vlen - 1] == '"') {
            val[vlen - 1] = '\0';
            val++;
        }

        snprintf(out, outlen, "%s", val);
        found = 0;
        break;
    }

    fclose(fp);
    return found;
}

void check_system(report_t *rep)
{
    report_section(rep, "System & kernel");

    /* --- uname(2): kernel identity from the syscall. --- */
    struct utsname u;
    if (uname(&u) == 0) {
        report_kv(rep, "Hostname", u.nodename);
        report_kv(rep, "Kernel",   u.release);
        report_kv(rep, "Arch",     u.machine);

        char line[256];
        snprintf(line, sizeof line, "%s %s", u.sysname, u.version);
        report_kv(rep, "System", line);
    } else {
        report_skip(rep, "uname(2) failed; skipping kernel identity");
    }

    /* --- Distribution name from /etc/os-release. --- */
    char distro[256];
    if (os_release_value("PRETTY_NAME", distro, sizeof distro) == 0)
        report_kv(rep, "Distro", distro);
    else
        report_skip(rep, "could not read /etc/os-release");

    /* --- Exact kernel build string from /proc/version. --- */
    char version[512];
    if (util_read_first_line("/proc/version", version, sizeof version) == 0) {
        report_item(rep, SEV_INFO, "Kernel build: %s", version);
        report_detail(rep,
                      "Cross-check the release against known local-privesc "
                      "advisories for this distro.");
    } else {
        report_skip(rep, "could not read /proc/version");
    }
}
