// parser.h – request-parser prototypes

#ifndef PARSER_H
#define PARSER_H

typedef struct {
    char method[8];      /* e.g. "GET" */
    char uri[256];       /* e.g. "/index.html" */
    char path[256];      /* e.g. "contents/index.html" */
    int  is_cgi;         /* 1 = dynamic, 0 = static */
} http_request_t;

/* Parses a single HTTP request from client_fd.
   Fills `req` and returns 0 on success.
   On malformed input, sends a 400 response and returns -1. */
int parse_request(int client_fd, http_request_t *req,
                  const char *root_dir, const char *index_file);

#endif
