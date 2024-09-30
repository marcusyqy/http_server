#ifndef _OS_WIN32_NET_H_
#define _OS_WIN32_NET_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include "base.h"
#include "os/common.h"


typedef struct {
  SOCKET socket;
} NetConnection;

NetInitResult os_net_init(void);
void os_net_exit(void);

NetConnection *os_net_start_connection(const char *address, int port);
void os_net_end_connection(NetConnection *connection);

#endif // _OS_WIN32_NET_H_

