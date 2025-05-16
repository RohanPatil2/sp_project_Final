// conf.h – configuration parser prototypes
#ifndef CONF_H
#define CONF_H

typedef struct {
    int  workers;          /* max simultaneous connections  */
    int  port;             /* listening port                */
    char root[128];        /* document root e.g. "contents/" */
    char index[128];       /* default index file            */
} config_t;

/* Reads key=value pairs from `file`. On success returns 0 and fills `cfg`.
   Returns -1 on I/O error, -2 on syntax error or missing required keys. */
int conf_load(const char *file, config_t *cfg);

#endif
