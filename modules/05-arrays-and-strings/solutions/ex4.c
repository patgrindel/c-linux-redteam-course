/* Exercise 4 — count words with strtok.
   strtok treats any run of delimiter chars as one separator and skips leading
   ones, so extra spaces do not inflate the count. */
#include <stdio.h>
#include <string.h>

int main(void) {
    char sentence[] = "  the  quick brown  fox ";
    int words = 0;

    char *tok = strtok(sentence, " \t\n");
    while (tok != NULL) {
        words++;
        tok = strtok(NULL, " \t\n");
    }

    printf("%d words\n", words);   /* expect 4 */
    return 0;
}
