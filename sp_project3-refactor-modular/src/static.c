// static.c – static-file server implementation
#include <fcntl.h>
#include <string.h>
#include <stdio.h> 
#include <sys/stat.h>
#include <unistd.h>

#ifdef __APPLE__
  #include <sys/socket.h>
#else
  #include <sys/sendfile.h>
#endif

#include "static.h"
#include "log.h"

int serve_static(int fd, const char *path)
{
    /* 404 if file is missing */
    if (access(path, F_OK) < 0) {
        const char *msg =
          "HTTP/1.0 404 Not Found\r\n"
          "Content-Type: text/plain\r\n\r\n"
          "404 File Not Found";
        send(fd, msg, strlen(msg), 0);
        write_to_log_file(-1, path, "404");
        return -1;
    }

    int file_fd = open(path, O_RDONLY);
    if (file_fd < 0) {
        write_to_log_file(-2, "open failed", "500");
        return -1;
    }

    struct stat st;
    fstat(file_fd, &st);

    char header[128];
    int hlen = snprintf(header, sizeof header,
                        "HTTP/1.0 200 OK\r\n"
                        "Content-Length: %ld\r\n"
                        "Content-Type: text/html\r\n\r\n",
                        (long)st.st_size);
    send(fd, header, hlen, 0);

#ifdef __APPLE__
    off_t sent = 0;
    sendfile(file_fd, fd, 0, &sent, NULL, 0);
#else
    off_t off = 0;
    sendfile(fd, file_fd, &off, st.st_size);
#endif
    close(file_fd);
    write_to_log_file(1, path, "GET static");
    return 0;
}
