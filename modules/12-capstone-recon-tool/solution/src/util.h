/* util.h -- tiny helpers shared by more than one check. */
#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <sys/types.h>

/* Strip trailing newline/whitespace in place. Returns s for convenience. */
char *util_rstrip(char *s);

/* Read the first line of `path` into buf (NUL-terminated, newline stripped).
 * Returns 0 on success, -1 if the file could not be opened/read. */
int util_read_first_line(const char *path, char *buf, size_t buflen);

/* Pointer to the final path component ("/usr/bin/passwd" -> "passwd").
 * Returns a pointer inside `path`; does not allocate. */
const char *util_basename(const char *path);

/* Render a mode into an "ls -l" style 10-char string + NUL, e.g.
 * "-rwsr-xr-x". `out` must be at least 11 bytes. */
void util_mode_str(mode_t mode, char out[11]);

#endif /* UTIL_H */
