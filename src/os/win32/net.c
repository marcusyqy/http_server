#include "net.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

static WSADATA wsa_data = {0};

NetInitResult os_win32_net_init(void) {
  WSADATA wsa_data = {0};
  int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
  switch(result) {
    case 0:
      return NetInitResult_Ok;
    case WSASYSNOTREADY:
      return NetInitResult_NotReady;
    case WSAVERNOTSUPPORTED:
      return NetInitResult_NotSupported;
    case WSAEINPROGRESS:
      return NetInitResult_AlreadyInitialized;
      break;
    case WSAEPROCLIM:
      return NetInitResult_LimitReached;
      break;
    case WSAEFAULT:
    default:
      return NetInitResult_Unknown;
  }
}

void os_win32_net_exit(void) {
  WSACleanup();
}
