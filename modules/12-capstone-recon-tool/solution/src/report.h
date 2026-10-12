/* report.h -- output helpers plus the small dynamic string/vector from Module 07.
 *
 * Everything the tool prints goes through this module. Keeping output in one
 * place means we can add colour, counters, or a future JSON mode without
 * touching the individual checks.
 */
#ifndef REPORT_H
#define REPORT_H

#include <stddef.h>
#include <stdio.h>

/* Portable printf-format checking: lets -Wall catch format/argument mismatches
 * in our variadic helpers. A no-op on compilers without the attribute. */
#if defined(__GNUC__)
#  define RECON_PRINTF_FMT(fmt_idx, first_arg) \
       __attribute__((format(printf, fmt_idx, first_arg)))
#else
#  define RECON_PRINTF_FMT(fmt_idx, first_arg)
#endif

/* ---------------------------------------------------------------------------
 * strbuf_t -- a growable, NUL-terminated string buffer.
 *
 * This is the "growable buffer" from Module 07, generalised a little. We own
 * the heap block in `data`; the caller must pair every strbuf_init() with a
 * strbuf_free().
 * ------------------------------------------------------------------------- */
typedef struct {
    char  *data;   /* NUL-terminated; valid (never NULL) after strbuf_init */
    size_t len;    /* bytes in use, not counting the terminating NUL */
    size_t cap;    /* bytes allocated for `data` */
} strbuf_t;

int  strbuf_init(strbuf_t *sb);                 /* 0 on success, -1 on OOM */
void strbuf_free(strbuf_t *sb);                 /* safe to call twice */
void strbuf_reset(strbuf_t *sb);                /* len = 0, keeps the buffer */
int  strbuf_append(strbuf_t *sb, const char *s);
int  strbuf_append_n(strbuf_t *sb, const char *s, size_t n);
int  strbuf_appendf(strbuf_t *sb, const char *fmt, ...) RECON_PRINTF_FMT(2, 3);

/* ---------------------------------------------------------------------------
 * strvec_t -- a growable array of owned C strings.
 *
 * push() copies the string in; free() releases every element and the array.
 * We use it to collect findings so a check can sort them before printing.
 * ------------------------------------------------------------------------- */
typedef struct {
    char **items;
    size_t len;
    size_t cap;
} strvec_t;

int  strvec_init(strvec_t *v);
void strvec_free(strvec_t *v);                  /* frees each item too */
int  strvec_push(strvec_t *v, const char *s);   /* copies s; -1 on OOM */
void strvec_sort(strvec_t *v);                  /* lexicographic, in place */

/* ---------------------------------------------------------------------------
 * The report context and severity levels.
 *
 * Severity is framed for an *assessor* reading the output:
 *   INFO  -- a neutral fact about the host.
 *   NOTE  -- worth a second look during an authorized assessment.
 *   WARN  -- a posture weakness that commonly matters (e.g. a privesc vector).
 * ------------------------------------------------------------------------- */
typedef enum {
    SEV_INFO = 0,
    SEV_NOTE,
    SEV_WARN
} severity_t;

typedef struct {
    FILE  *out;         /* where output goes (stdout or a file) */
    int    use_color;   /* 1 = emit ANSI colour codes */
    int    verbose;     /* 1 = print extra detail and skipped-scan notes */
    size_t n_info;      /* running counts, shown in the summary */
    size_t n_note;
    size_t n_warn;
} report_t;

void report_init(report_t *rep, FILE *out, int use_color, int verbose);

/* A bold section banner, e.g. "== Users & accounts ==". */
void report_section(report_t *rep, const char *title);

/* A simple "key: value" line, indented under the current section. */
void report_kv(report_t *rep, const char *key, const char *value);

/* A finding. Counts toward the summary according to `sev`. */
void report_item(report_t *rep, severity_t sev, const char *fmt, ...)
    RECON_PRINTF_FMT(3, 4);

/* An indented, dimmed sub-line -- used for the "why this matters to an
 * assessor" explanation that follows a finding. Not counted. */
void report_detail(report_t *rep, const char *fmt, ...) RECON_PRINTF_FMT(2, 3);

/* A degraded-scan notice (missing file, permission denied). Dimmed, printed
 * only in verbose mode, and NOT counted as a finding -- a check that cannot
 * read something should degrade, not fail. */
void report_skip(report_t *rep, const char *fmt, ...) RECON_PRINTF_FMT(2, 3);

/* Final tally of INFO/NOTE/WARN items. */
void report_summary(report_t *rep);

#endif /* REPORT_H */
