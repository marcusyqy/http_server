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
} OsWin32_NetConnection;

NetInitResult os_win32_net_init(void);
void os_win32_net_exit(void);

OsWin32_NetConnection *os_win32_net_start_connection(const char *address, int port, NetConnectionResult *result);
void os_win32_net_end_connection(OsWin32_NetConnection *connection);

#endif // _OS_WIN32_NET_H_

