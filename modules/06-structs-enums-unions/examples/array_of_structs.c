#include <stdio.h>

/* An array of structs is the standard way to hold a table of records -- for
   example, the open ports you found while scanning a host you are assessing. */
typedef struct {
    const char *ip;
    int         port;
    const char *service;
} Finding;

int main(void) {
    Finding findings[3] = {
        {"10.0.0.5",  22,  "ssh"},
        {"10.0.0.5",  80,  "http"},
        {"10.0.0.9",  445, "smb"},
    };
    size_t n = sizeof findings / sizeof findings[0];

    for (size_t i = 0; i < n; i++) {
        printf("%-10s port %-4d %s\n",
               findings[i].ip, findings[i].port, findings[i].service);
    }
    return 0;
}
