/* checks_worldwritable.c -- flag world-writable files and directories.
 *
 * Why an assessor cares: a world-writable file can be tampered with by any
 * local user (replace a script a cron job runs, swap a config). A world-
 * writable directory *without* the sticky bit lets anyone delete or replace
 * other users' files in it -- /tmp is the canonical safe case (it has the
 * sticky bit); a world-writable /etc subdirectory is not.
 *
 * Technique: nftw(3) again, but depth-limited with FTW_SKIP_SUBTREE so we
 * don't spend minutes under /home or /var. We test S_IWOTH on each entry.
 */
#define _GNU_SOURCE
#include "checks_worldwritable.h"
#include "util.h"

#include <ftw.h>
#include <sys/stat.h>

/* A handful of directories where a world-writable entry is genuinely
 * interesting. We deliberately avoid walking all of / for speed. */
static const char *const WW_DIRS[] = {
    "/etc", "/var/tmp", "/dev/shm", "/usr/local", "/opt", "/srv",
};

#define WW_MAX_DEPTH  4      /* nftw level at which we stop descending */
#define WW_MAX_HITS   200    /* keep output bounded on a messy box */

static report_t *g_rep;
static unsigned  g_hits;

static int ww_cb(const char *fpath, const struct stat *sb,
                 int typeflag, struct FTW *ftwbuf)
{
    if (typeflag == FTW_NS || typeflag == FTW_DNR)
        return FTW_CONTINUE;

    /* Don't descend past our depth cap (the cap applies to directories). */
    if (typeflag == FTW_D && ftwbuf->level >= WW_MAX_DEPTH)
        return FTW_SKIP_SUBTREE;

    if (g_hits >= (unsigned)WW_MAX_HITS)
        return FTW_CONTINUE;

    /* Symlinks (FTW_SL under FTW_PHYS) carry misleading permission bits; the
     * target is what matters, so skip them here. */
    if (typeflag != FTW_F && typeflag != FTW_D)
        return FTW_CONTINUE;

    if (!(sb->st_mode & S_IWOTH))
        return FTW_CONTINUE;

    char modestr[11];
    util_mode_str(sb->st_mode, modestr);

    if (typeflag == FTW_D) {
        /* A sticky world-writable dir (like /tmp) is expected; without sticky
         * it is a real problem. */
        int sticky = (sb->st_mode & S_ISVTX) != 0;
        report_item(g_rep, sticky ? SEV_NOTE : SEV_WARN,
                    "%s %s%s", modestr, fpath,
                    sticky ? " (sticky)" : " (NO sticky bit)");
    } else {
        report_item(g_rep, SEV_WARN, "%s %s", modestr, fpath);
    }

    g_hits++;
    return FTW_CONTINUE;
}

void check_worldwritable(report_t *rep)
{
    report_section(rep, "World-writable paths");

    g_rep = rep;
    g_hits = 0;

    for (size_t i = 0; i < sizeof WW_DIRS / sizeof WW_DIRS[0]; i++) {
        int rc = nftw(WW_DIRS[i], ww_cb, 16, FTW_PHYS | FTW_ACTIONRETVAL);
        if (rc == -1)
            report_skip(rep, "could not scan %s", WW_DIRS[i]);
    }

    if (g_hits == 0)
        report_item(rep, SEV_INFO, "no world-writable entries in scanned dirs");
    else {
        if (g_hits >= (unsigned)WW_MAX_HITS)
            report_detail(rep, "output capped at %d entries; re-run focused on "
                               "one directory for the full list", WW_MAX_HITS);
        report_detail(rep,
                      "Writable files that root or a service reads/executes "
                      "are the ones to prioritise.");
    }

    g_rep = NULL;
}
