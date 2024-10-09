#ifndef _OS_WIN32_NET_H_
#define _OS_WIN32_NET_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include "os/net.common.h"

typedef struct {
  SOCKET socket;
  NetSocketType type;
} NetConnection;

#endif // _OS_WIN32_NET_H_

