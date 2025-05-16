// static.h – static-file server prototypes
#ifndef STATIC_H
#define STATIC_H

/* Streams the file at `filepath` to client socket `client_fd`.
   Returns 0 on success, -1 if it already sent a 404/500. */
int serve_static(int client_fd, const char *filepath);

#endif  /* STATIC_H */
