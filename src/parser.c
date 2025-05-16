// parser.c – request-parser implementation


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "parser.h"
#include "log.h"

int parse_request(int fd, http_request_t *req,
                  const char *root_dir, const char *index_file)
{
    char buf[2048] = {0};
    ssize_t n = recv(fd, buf, sizeof buf - 1, 0);
    if (n <= 0) return -1;

    /* first line: METHOD URI HTTP/1.x */
    char *method = strtok(buf,  " \r\n");
    char *uri    = strtok(NULL, " \r\n");
    if (!method || !uri) {
        const char *bad = "HTTP/1.0 400 Bad Request\r\n\r\n";
        send(fd, bad, strlen(bad), 0);
        write_to_log_file(-1, "Malformed request", "400");
        return -1;
    }

    /* copy to struct */
    strncpy(req->method, method, sizeof req->method - 1);

    if (strcmp(uri, "/") == 0)
        uri = (char *)index_file;          /* default index */

    strncpy(req->uri, uri, sizeof req->uri - 1);

    /* build absolute path */
    snprintf(req->path, sizeof req->path, "%s%s",
             root_dir, uri + (uri[0] == '/'));

    req->is_cgi = strstr(uri, ".cgi") != NULL ||
                  strstr(uri, "/cgi-bin/") != NULL;

    return 0;
}
