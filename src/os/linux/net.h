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
} OsLinux_NetConnection;

NetInitResult os_linux_net_init(void);
void os_linux_net_exit(void);

OsLinux_NetConnection *os_linux_net_start_connection(const char *address, int port, NetConnectionResult *result);
void os_linux_net_end_connection(OsLinux_NetConnection *connection);


#endif //  _OS_LINUX_NET_H_
