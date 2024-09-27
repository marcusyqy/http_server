#ifndef _OS_WIN32_NET_H_
#define _OS_WIN32_NET_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#include "base/dtype.h"

typedef s32 NetInitResult;

enum {
	NetInitResult_Ok = 0,
	NetInitResult_NotReady,
	NetInitResult_NotSupported, // not suppported on this architecture.
	NetInitResult_AlreadyInitialized,
	NetInitResult_LimitReached,
	NetInitResult_Unknown,
};

NetInitResult os_win32_net_init(void);
void          os_win32_net_exit(void);

#endif // _OS_WIN32_NET_H_

