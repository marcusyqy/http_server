#ifndef _OS_NET_H_
#define _OS_NET_H_

#include "base.h"

#if defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

typedef SOCKET NetSocket;
#elif defined(__linux__)

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef int NetSocket;
#else
#error not supported
#endif

enum {
  NetInitResult_Ok = 0,
  NetInitResult_NotReady,
  NetInitResult_NotSupported, // not suppported on this architecture.
  NetInitResult_AlreadyInitialized,
  NetInitResult_LimitReached,
  NetInitResult_Unknown,
};

enum {
  NetConnectionResult_Ok = 0,
};

enum {
  NetConnectionResult_Error = -1,
  NetConnectionResult_Disconnect = 0,
};


enum {
  NetSocketType_Invalid = -1,
  NetSocketType_None = 0,
  NetSocketType_SendRecv = 1 << 0,
};


typedef s32 NetConnectionRecvResult;
typedef s32 NetConnectionSendResult;
typedef s32 NetConnectionResult;
typedef s32 NetInitResult;
typedef s32 NetSocketType;


typedef struct {
  NetSocket socket;
  NetSocketType type;
} NetConnection;


NetInitResult os_net_init(void);
void os_net_exit(void);

NetConnection os_net_start_connection(const char *address, int port);
void os_net_listen(NetConnection connection[static 1], size_t max_connections);
NetConnection os_net_accept(NetConnection connection[static 1]);
bool os_net_connection_valid(NetConnection connection[static 1]);
void os_net_end_connection(NetConnection connection[static 1]);

NetConnectionRecvResult os_net_recv_sync(NetConnection connection[static 1], char buffer[static 1], size_t length);
NetConnectionSendResult os_net_send_sync(NetConnection connection[static 1], char buffer[static 1], size_t length);

/// @TODO: should we even bother with these? Or should we move it out of net
int os_last_error_code();
void os_print_last_error(const char msg[static 1]);

#endif // _OS_NET_H_
