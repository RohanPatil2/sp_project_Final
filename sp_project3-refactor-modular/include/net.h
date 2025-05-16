// net.h – networking prototypes
// net.h – networking prototypes

#ifndef NET_H
#define NET_H

/* Returns a listening socket fd bound to the given port (or -1 on error). */
int start_listening(int port, int backlog);

#endif
