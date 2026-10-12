/* report.c -- implementation of the output helpers and the M07 containers. */
#define _GNU_SOURCE
#include "report.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/* ---------------------------------------------------------------------------
 * strbuf_t
 * ------------------------------------------------------------------------- */

int strbuf_init(strbuf_t *sb)
{
    sb->cap = 64;
    sb->len = 0;
    sb->data = malloc(sb->cap);
    if (sb->data == NULL) {
        sb->cap = 0;
        return -1;
    }
    sb->data[0] = '\0';
    return 0;
}

void strbuf_free(strbuf_t *sb)
{
    if (sb == NULL)
        return;
    free(sb->data);
    sb->data = NULL;
    sb->len = 0;
    sb->cap = 0;
}

void strbuf_reset(strbuf_t *sb)
{
    sb->len = 0;
    if (sb->data != NULL)
        sb->data[0] = '\0';
}

/* Make sure there is room for `extra` more bytes plus the NUL. */
static int strbuf_ensure(strbuf_t *sb, size_t extra)
{
    size_t need = sb->len + extra + 1;
    if (need <= sb->cap)
        return 0;

    size_t new_cap = (sb->cap != 0) ? sb->cap : 64;
    while (new_cap < need)
        new_cap *= 2;

    char *p = realloc(sb->data, new_cap);
    if (p == NULL)
        return -1;          /* old buffer is still valid; caller can bail out */
    sb->data = p;
    sb->cap = new_cap;
    return 0;
}

int strbuf_append_n(strbuf_t *sb, const char *s, size_t n)
{
    if (strbuf_ensure(sb, n) != 0)
        return -1;
    memcpy(sb->data + sb->len, s, n);
    sb->len += n;
    sb->data[sb->len] = '\0';
    return 0;
}

int strbuf_append(strbuf_t *sb, const char *s)
{
    return strbuf_append_n(sb, s, strlen(s));
}

int strbuf_appendf(strbuf_t *sb, const char *fmt, ...)
{
    va_list ap;

    /* First pass: measure. vsnprintf returns the length it *would* write. */
    va_start(ap, fmt);
    int needed = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    if (needed < 0)
        return -1;

    if (strbuf_ensure(sb, (size_t)needed) != 0)
        return -1;

    /* Second pass: actually write into the guaranteed-large-enough buffer. */
    va_start(ap, fmt);
    int written = vsnprintf(sb->data + sb->len, (size_t)needed + 1, fmt, ap);
    va_end(ap);
    if (written < 0)
        return -1;

    sb->len += (size_t)written;
    return 0;
}

/* ---------------------------------------------------------------------------
 * strvec_t
 * ------------------------------------------------------------------------- */

int strvec_init(strvec_t *v)
{
    v->cap = 8;
    v->len = 0;
    v->items = malloc(v->cap * sizeof(*v->items));
    if (v->items == NULL) {
        v->cap = 0;
        return -1;
    }
    return 0;
}

void strvec_free(strvec_t *v)
{
    if (v == NULL || v->items == NULL)
        return;
    for (size_t i = 0; i < v->len; i++)
        free(v->items[i]);
    free(v->items);
    v->items = NULL;
    v->len = 0;
    v->cap = 0;
}

int strvec_push(strvec_t *v, const char *s)
{
    if (v->len == v->cap) {
        size_t new_cap = v->cap ? v->cap * 2 : 8;
        char **p = realloc(v->items, new_cap * sizeof(*v->items));
        if (p == NULL)
            return -1;
        v->items = p;
        v->cap = new_cap;
    }
    char *copy = strdup(s);
    if (copy == NULL)
        return -1;
    v->items[v->len++] = copy;
    return 0;
}

static int strvec_cmp(const void *a, const void *b)
{
    /* qsort hands us pointers *to* the array elements, i.e. char **. */
    const char *const *pa = a;
    const char *const *pb = b;
    return strcmp(*pa, *pb);
}

void strvec_sort(strvec_t *v)
{
    if (v->len > 1)
        qsort(v->items, v->len, sizeof(*v->items), strvec_cmp);
}

/* ---------------------------------------------------------------------------
 * report_t
 * ------------------------------------------------------------------------- */

/* ANSI colour codes, or empty strings when colour is disabled. */
#define C_RESET  "\033[0m"
#define C_BOLD   "\033[1m"
#define C_DIM    "\033[2m"
#define C_CYAN   "\033[36m"
#define C_YELLOW "\033[33m"
#define C_RED    "\033[31m"

static const char *col(const report_t *rep, const char *code)
{
    return rep->use_color ? code : "";
}

void report_init(report_t *rep, FILE *out, int use_color, int verbose)
{
    rep->out = out;
    rep->use_color = use_color;
    rep->verbose = verbose;
    rep->n_info = 0;
    rep->n_note = 0;
    rep->n_warn = 0;
}

void report_section(report_t *rep, const char *title)
{
    fprintf(rep->out, "\n%s== %s ==%s\n",
            col(rep, C_BOLD), title, col(rep, C_RESET));
}

void report_kv(report_t *rep, const char *key, const char *value)
{
    fprintf(rep->out, "    %s: %s\n", key, value);
}

void report_item(report_t *rep, severity_t sev, const char *fmt, ...)
{
    const char *tag;
    const char *color;

    switch (sev) {
    case SEV_WARN:
        tag = "[!]";
        color = C_RED;
        rep->n_warn++;
        break;
    case SEV_NOTE:
        tag = "[+]";
        color = C_YELLOW;
        rep->n_note++;
        break;
    case SEV_INFO:
    default:
        tag = "[*]";
        color = C_CYAN;
        rep->n_info++;
        break;
    }

    fprintf(rep->out, "  %s%s%s ", col(rep, color), tag, col(rep, C_RESET));

    va_list ap;
    va_start(ap, fmt);
    vfprintf(rep->out, fmt, ap);
    va_end(ap);

    fputc('\n', rep->out);
}

void report_detail(report_t *rep, const char *fmt, ...)
{
    fprintf(rep->out, "      %s", col(rep, C_DIM));

    va_list ap;
    va_start(ap, fmt);
    vfprintf(rep->out, fmt, ap);
    va_end(ap);

    fprintf(rep->out, "%s\n", col(rep, C_RESET));
}

void report_skip(report_t *rep, const char *fmt, ...)
{
    if (!rep->verbose)
        return;

    fprintf(rep->out, "  %s[-] ", col(rep, C_DIM));

    va_list ap;
    va_start(ap, fmt);
    vfprintf(rep->out, fmt, ap);
    va_end(ap);

    fprintf(rep->out, "%s\n", col(rep, C_RESET));
}

void report_summary(report_t *rep)
{
    report_section(rep, "Summary");
    fprintf(rep->out,
            "    %zu info, %zu note%s, %zu warning%s\n",
            rep->n_info,
            rep->n_note, rep->n_note == 1 ? "" : "s",
            rep->n_warn, rep->n_warn == 1 ? "" : "s");
    report_detail(rep,
                  "Counts are a starting point, not a score -- read the "
                  "findings in context.");
}
