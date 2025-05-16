// net.c – networking implementation
// net.c – networking implementation


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "net.h"

int start_listening(int port, int backlog)
{
    int listenfd;
    struct sockaddr_in servaddr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    int optval = 1;
    /* Allow quick restart after crash: */
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                   &optval, sizeof(optval)) < 0) {
        perror("setsockopt");
        close(listenfd);
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(port);

    if (bind(listenfd, (struct sockaddr *)&servaddr,
             sizeof(servaddr)) < 0) {
        perror("bind");
        close(listenfd);
        return -1;
    }

    if (listen(listenfd, backlog) < 0) {
        perror("listen");
        close(listenfd);
        return -1;
    }

    return listenfd;
}
