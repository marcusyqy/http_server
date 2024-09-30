#ifndef _OS_LINUX_NET_H_
#define _OS_LINUX_NET_H_

#include "os/common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct {
  int socket;
  struct sockaddr_in meta;
} Os_NetConnection;

NetInitResult os_net_init(void);
void os_net_exit(void);

Os_NetConnection *os_net_start_connection(const char *address, int port);
void os_net_end_connection(Os_NetConnection *connection);

void os_listen(Os_NetConnection *connection, size_t max_connections);
Os_NetConnection *os_accept(Os_NetConnection *connection);


#endif //  _OS_LINUX_NET_H_
