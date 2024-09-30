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
} NetConnection;

NetInitResult os_net_init(void);
void os_net_exit(void);

NetConnection *os_net_start_connection(const char *address, int port);
void os_net_end_connection(NetConnection *connection);

void os_net_listen(NetConnection *connection, size_t max_connections);

NetConnection *os_net_accept(NetConnection *connection);

NetConnectionRecvResult os_net_recv_sync(NetConnection *connection, char *buffer, size_t length);

NetConnectionSendResult os_net_send_sync(NetConnection *connection, char *buffer, size_t length);

#endif //  _OS_LINUX_NET_H_
