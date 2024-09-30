#ifndef _OS_NET_H_
#define _OS_NET_H_

#include "common.h"

#if defined(_WIN32)
#include "win32/net.h"
#else
#include "linux/net.h"
#endif

NetInitResult os_net_init(void);
void os_net_exit(void);

NetConnection *os_net_start_connection(const char *address, int port);
void os_net_end_connection(NetConnection *connection);

void os_net_listen(NetConnection *connection, size_t max_connections);

NetConnection *os_net_accept(NetConnection *connection);

NetConnectionRecvResult os_net_recv_sync(NetConnection *connection, char *buffer, size_t length);
NetConnectionSendResult os_net_send_sync(NetConnection *connection, char *buffer, size_t length);

#endif // _OS_NET_H_
