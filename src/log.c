// log.c – logging implementation
#include <stdio.h>
#include <time.h>
#include "log.h"

void write_to_log_file(int log_type, const char *content, const char *tag)
{
    char ts[64];
    time_t now = time(NULL);
    strftime(ts, sizeof ts, "%c", localtime(&now));

    FILE *lf = (log_type > -1)
                 ? fopen("logs/access.log", "a")
                 : fopen("logs/error.log",  "a");

    if (lf) {
        fprintf(lf, "[%s] %s: %s\n", ts, tag, content);
        fclose(lf);
    }
    if (log_type == -2)
        fprintf(stderr, "%s\n", content);
}
