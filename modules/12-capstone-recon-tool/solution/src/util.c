/* util.c -- implementation of the shared helpers. */
#define _GNU_SOURCE
#include "util.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

char *util_rstrip(char *s)
{
    size_t n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1]))
        s[--n] = '\0';
    return s;
}

int util_read_first_line(const char *path, char *buf, size_t buflen)
{
    if (buflen == 0)
        return -1;

    FILE *fp = fopen(path, "r");
    if (fp == NULL)
        return -1;

    if (fgets(buf, (int)buflen, fp) == NULL) {
        fclose(fp);
        buf[0] = '\0';
        return -1;
    }
    fclose(fp);

    util_rstrip(buf);
    return 0;
}

const char *util_basename(const char *path)
{
    const char *slash = strrchr(path, '/');
    return slash ? slash + 1 : path;
}

void util_mode_str(mode_t mode, char out[11])
{
    /* File type character. */
    if (S_ISDIR(mode))       out[0] = 'd';
    else if (S_ISLNK(mode))  out[0] = 'l';
    else if (S_ISCHR(mode))  out[0] = 'c';
    else if (S_ISBLK(mode))  out[0] = 'b';
    else if (S_ISFIFO(mode)) out[0] = 'p';
    else if (S_ISSOCK(mode)) out[0] = 's';
    else                     out[0] = '-';

    out[1] = (mode & S_IRUSR) ? 'r' : '-';
    out[2] = (mode & S_IWUSR) ? 'w' : '-';
    out[3] = (mode & S_IXUSR) ? 'x' : '-';
    out[4] = (mode & S_IRGRP) ? 'r' : '-';
    out[5] = (mode & S_IWGRP) ? 'w' : '-';
    out[6] = (mode & S_IXGRP) ? 'x' : '-';
    out[7] = (mode & S_IROTH) ? 'r' : '-';
    out[8] = (mode & S_IWOTH) ? 'w' : '-';
    out[9] = (mode & S_IXOTH) ? 'x' : '-';

    /* Overlay the special bits the way ls does. */
    if (mode & S_ISUID)
        out[3] = (mode & S_IXUSR) ? 's' : 'S';
    if (mode & S_ISGID)
        out[6] = (mode & S_IXGRP) ? 's' : 'S';
    if (mode & S_ISVTX)
        out[9] = (mode & S_IXOTH) ? 't' : 'T';

    out[10] = '\0';
}
