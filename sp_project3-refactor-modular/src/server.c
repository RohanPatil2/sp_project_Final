/* src/server.c */

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "net.h"        /* start_listening() */
#include "log.h"        /* write_to_log_file */
#include "static.h"     /* serve_static()    */
#include "parser.h"     /* parse_request()   */
#include "conf.h"       /* conf_load()       */

#ifdef __APPLE__
  #include <sys/socket.h>
#else
  #include <sys/sendfile.h>
#endif

/* ------------------------------------------------------------------ */
/*                global state copied from config                     */
char root_directory[128];
char index_filename[128];
/* ------------------------------------------------------------------ */

/* ---------- CGI launcher ------------------------------------------ */
static int send_to_cgi(int sock_fd,
                       const char *method,
                       const char *script_path)
{
    dup2(sock_fd, STDOUT_FILENO);          /* CGI stdout → client socket */
    char *argv[] = { (char *)script_path, NULL };
    setenv("QUERY_STRING", script_path, 1);

    int rc = (strcmp(method, "POST") == 0)
               ? execv("cgi-bin/POST.cgi", argv)
               : execv("cgi-bin/GET.cgi",  argv);

    if (rc < 0) {
        write_to_log_file(-2, "execv failed", "ERROR 500");
        const char *msg =
            "HTTP/1.0 500 Internal Server Error\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 25\r\n\r\n"
            "500 Internal Server Error";
        send(sock_fd, msg, strlen(msg), 0);
    }
    return 0;
}

/* ---------- handle a single client connection -------------------- */
static void handle_client(int sock_fd)
{
    http_request_t req;
    if (parse_request(sock_fd, &req,
                      root_directory, index_filename) < 0)
        return;                                   /* parse_request sent 400 */

    if (!req.is_cgi) {                            /* STATIC */
        serve_static(sock_fd, req.path);
    } else {                                      /* CGI */
        write_to_log_file(1, req.uri, "GET CGI");
        send_to_cgi(sock_fd, req.method, req.path);
    }
}

/* =============================  main  ============================= */
int main(void)
{
    /* -------- load config -------- */
    config_t cfg;
    if (conf_load("conf/httpd.conf", &cfg) != 0) {
        /* conf_load already logged error */
        return EXIT_FAILURE;
    }

    /* copy config fields into globals for legacy code */
    strcpy(root_directory, cfg.root);
    strcpy(index_filename, cfg.index);
    int max_conn = cfg.workers;
    int port     = cfg.port;

    printf("\nLoaded configuration:\n"
           "  Connections: %d\n"
           "  Root dir:    %s\n"
           "  Index file:  %s\n"
           "  Port:        %d\n\n",
           max_conn, root_directory, index_filename, port);

    /* -------- networking setup -------- */
    int listenfd = start_listening(port, max_conn);
    if (listenfd < 0) {
        write_to_log_file(-2, "socket/listen failed", "FATAL");
        return EXIT_FAILURE;
    }

    /* -------- accept loop -------- */
    for (;;) {
        struct sockaddr_in cli;
        socklen_t len = sizeof cli;
        int cli_fd = accept(listenfd, (struct sockaddr *)&cli, &len);
        if (cli_fd < 0) {
            write_to_log_file(-2, "accept failed", "ERROR");
            continue;
        }
        if (fork() == 0) {               /* child */
            close(listenfd);
            handle_client(cli_fd);
            close(cli_fd);
            _exit(0);
        }
        close(cli_fd);                   /* parent */
    }
}
