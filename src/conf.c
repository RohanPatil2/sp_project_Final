// conf.c – configuration parser implementation
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "conf.h"
#include "log.h"

/* helper: trim leading/trailing whitespace */
static char *trim(char *s)
{
    while (isspace(*s)) ++s;
    if (*s == 0) return s;
    char *e = s + strlen(s) - 1;
    while (e > s && isspace(*e)) *e-- = '\0';
    return s;
}

int conf_load(const char *file, config_t *cfg)
{
    FILE *fp = fopen(file, "r");
    if (!fp) {
        write_to_log_file(-2, "conf_load: cannot open file", "ERROR");
        return -1;
    }

    /* set defaults in case some keys are missing */
    cfg->workers = 5;
    cfg->port    = 8080;
    strcpy(cfg->root,  "contents/");
    strcpy(cfg->index, "index.html");

    char line[256];
    while (fgets(line, sizeof line, fp)) {
        if (line[0] == '#' || line[0] == '\n') continue;   /* skip comments */
        char *eq = strchr(line, '=');
        if (!eq) { fclose(fp); return -2; }
        *eq = '\0';
        char *key = trim(line);
        char *val = trim(eq + 1);

        if      (!strcmp(key, "workers")) cfg->workers = atoi(val);
        else if (!strcmp(key, "port"))    cfg->port    = atoi(val);
        else if (!strcmp(key, "root"))    strncpy(cfg->root,  val, sizeof cfg->root  - 1);
        else if (!strcmp(key, "index"))   strncpy(cfg->index, val, sizeof cfg->index - 1);
        /* unknown keys are ignored */
    }
    fclose(fp);
    return 0;
}
