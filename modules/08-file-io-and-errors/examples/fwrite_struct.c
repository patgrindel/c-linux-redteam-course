/* fwrite_struct.c - write an array of structs to a binary file and read it back.
 *
 * fwrite/fread move fixed-size records and return a RECORD count (not bytes).
 * A file written this way is not portable across machines (padding, word size,
 * byte order) - fine for a scratch file on one host.
 *
 * Build: gcc -std=c11 -Wall -Wextra fwrite_struct.c -o fwrite_struct
 * Run:   ./fwrite_struct
 */
#include <stdio.h>

typedef struct {
    int  id;
    char name[16];
} record_t;

int main(void) {
    record_t out[2] = { {1, "alpha"}, {2, "beta"} };

    FILE *f = fopen("/tmp/records.bin", "wb");
    if (f == NULL) {
        perror("open for write");
        return 1;
    }
    size_t w = fwrite(out, sizeof(record_t), 2, f);
    if (w != 2) {
        fprintf(stderr, "short write (%zu)\n", w);
        fclose(f);
        return 1;
    }
    fclose(f);

    record_t in[2];
    f = fopen("/tmp/records.bin", "rb");
    if (f == NULL) {
        perror("open for read");
        return 1;
    }
    size_t r = fread(in, sizeof(record_t), 2, f);
    if (r != 2) {
        fprintf(stderr, "short read (%zu)\n", r);
        fclose(f);
        return 1;
    }
    fclose(f);

    for (int i = 0; i < 2; i++) {
        printf("id=%d name=%s\n", in[i].id, in[i].name);
    }
    return 0;
}
