/* checks_suid.c -- find SUID/SGID executables in the usual binary directories.
 *
 * Why an assessor cares: a SUID-root binary runs with root's privileges no
 * matter who launches it. A *non-standard* or known-vulnerable SUID binary is
 * one of the most common local-privesc vectors, so we list what's there and
 * highlight anything that isn't part of the usual distro set.
 *
 * Technique: nftw(3) walks a directory tree and hands us a stat buffer for
 * each entry, so we can test the mode bits without a second stat(2) call
 * (Module 08/09). FTW_PHYS means "don't follow symlinks". nftw has no
 * user-data argument, so the report context is passed through a file-scope
 * pointer -- fine here because the tool is single-threaded.
 */
#define _GNU_SOURCE
#include "checks_suid.h"
#include "util.h"

#include <ftw.h>
#include <string.h>
#include <sys/stat.h>

/* Directories we actually expect SUID binaries to live in. Walking the whole
 * filesystem is possible but slow and noisy; these cover the real cases. */
static const char *const SUID_DIRS[] = {
    "/bin", "/sbin", "/usr/bin", "/usr/sbin",
    "/usr/local/bin", "/usr/local/sbin", "/opt",
};

/* SUID/SGID binaries that ship with a stock distro. Anything outside this set
 * gets upgraded to a NOTE so it stands out. The list is intentionally short --
 * the point is "is this one surprising?", not an exhaustive allowlist. */
static const char *const COMMON_SUID[] = {
    "passwd", "sudo", "su", "mount", "umount", "ping", "ping6",
    "chsh", "chfn", "newgrp", "gpasswd", "pkexec", "fusermount",
    "fusermount3", "ssh-keysign", "dbus-daemon-launch-helper",
    "unix_chkpwd", "pam_timestamp_check", "write", "wall",
    "crontab", "at", "expiry", "chage", "sg", "umount.udisks2",
};

static report_t *g_rep;              /* current report for the nftw callback */
static unsigned  g_suid_count;       /* files flagged during this walk */

static int is_common(const char *base)
{
    for (size_t i = 0; i < sizeof COMMON_SUID / sizeof COMMON_SUID[0]; i++)
        if (strcmp(base, COMMON_SUID[i]) == 0)
            return 1;
    return 0;
}

static int suid_cb(const char *fpath, const struct stat *sb,
                   int typeflag, struct FTW *ftwbuf)
{
    (void)ftwbuf;

    /* FTW_NS = stat failed (e.g. permission denied); FTW_DNR = unreadable
     * directory. Degrade quietly: just don't descend/report. */
    if (typeflag == FTW_NS || typeflag == FTW_DNR)
        return FTW_CONTINUE;

    if (typeflag != FTW_F)
        return FTW_CONTINUE;         /* only regular files can be SUID execs */

    if (!(sb->st_mode & (S_ISUID | S_ISGID)))
        return FTW_CONTINUE;

    char modestr[11];
    util_mode_str(sb->st_mode, modestr);

    const char *base = util_basename(fpath);
    int surprising = !is_common(base);

    report_item(g_rep, surprising ? SEV_NOTE : SEV_INFO,
                "%s %s (uid %u)", modestr, fpath, (unsigned)sb->st_uid);

    g_suid_count++;
    return FTW_CONTINUE;
}

void check_suid(report_t *rep)
{
    report_section(rep, "SUID / SGID binaries");

    g_rep = rep;
    g_suid_count = 0;

    for (size_t i = 0; i < sizeof SUID_DIRS / sizeof SUID_DIRS[0]; i++) {
        /* FTW_ACTIONRETVAL lets the callback return FTW_CONTINUE/FTW_SKIP_*;
         * FTW_PHYS keeps us from following symlinks out of the tree. */
        int rc = nftw(SUID_DIRS[i], suid_cb, 16, FTW_PHYS | FTW_ACTIONRETVAL);
        if (rc == -1)
            report_skip(rep, "could not scan %s", SUID_DIRS[i]);
    }

    if (g_suid_count == 0)
        report_item(rep, SEV_INFO, "no SUID/SGID files found in common paths");
    else
        report_detail(rep,
                      "Check any [+] entry against GTFOBins-style known-abuse "
                      "notes before relying on it as a vector.");

    g_rep = NULL;
}
