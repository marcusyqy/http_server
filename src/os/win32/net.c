#include "net.h"
#include "os/net.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#pragma comment(lib, "Ws2_32.lib")

static WSADATA wsa_data = {0};

NetInitResult os_net_init(void) {
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

void os_net_exit(void) {
  WSACleanup();
}

NetConnection * os_net_start_connection(const char *address, int port) {
  struct addrinfo *addr_result = NULL;
  struct addrinfo *addr_ptr    = NULL;
  struct addrinfo  addr_hints  = {0};

  ZeroMemory(&addr_hints, sizeof(addr_hints));
  addr_hints.ai_family   = AF_INET;      // using ipv4
  addr_hints.ai_socktype = SOCK_STREAM;  // specify stream socket
  addr_hints.ai_protocol = IPPROTO_TCP;  // TCP protocol
  addr_hints.ai_flags    = AI_PASSIVE;   //

  // Resolve the local address and port to be used by the server
  char port_buffer[10] = {0};
  sprintf(port_buffer, "%d", port);
  int call_result = getaddrinfo(address, port_buffer, &addr_hints, &addr_result);
  if(call_result != 0) {
    printf("getaddrinfo failed: %d\n", call_result);
    return NULL;
  }

  SOCKET listen_socket = socket(addr_result->ai_family, addr_result->ai_socktype, addr_result->ai_protocol);
  if(listen_socket == INVALID_SOCKET) {
    printf("Error at socket(): %d\n", WSAGetLastError());
    freeaddrinfo(addr_result);
    return NULL;
  }

  call_result = bind(listen_socket, addr_result->ai_addr, (int)addr_result->ai_addrlen);
  if(call_result == SOCKET_ERROR) {
    printf("bind failed with error: %d\n", WSAGetLastError());
    freeaddrinfo(addr_result);
    closesocket(listen_socket);
    os_net_exit();
    return NULL;
  }

  freeaddrinfo(addr_result);

  NetConnection *ret = malloc(sizeof(NetConnection));
  ret->socket = listen_socket;
  ret->type = NetSocketType_None;
  return ret;
}

void os_net_end_connection(NetConnection *connection) {
  assert(connection);
  if(connection->type & NetSocketType_SendRecv) {
    int end_result = shutdown(connection->socket, SD_SEND);
    if(end_result == SOCKET_ERROR) {
      printf("shutdown failed: %d\n", WSAGetLastError());
    }
  }
  closesocket(connection->socket);
  free(connection);
}

void os_net_listen(NetConnection *connection, size_t max_connections) {
  /// @TODO: we have to change this.
  /// usually we while this but here we need to do something smarter.
  if(listen(connection->socket, SOMAXCONN) == SOCKET_ERROR) {
    fprintf(stdout, "%s", "Something went wrong during os_net_listen");
  }
}

NetConnection *os_net_accept(NetConnection *connection) {
    SOCKET client_socket = accept(connection->socket, NULL, NULL);
    NetConnection *new_connection = malloc(sizeof(NetConnection));
    new_connection->socket = client_socket;
    new_connection->type = NetSocketType_SendRecv;
    return new_connection;
}

NetConnectionRecvResult os_net_recv_sync(NetConnection *connection, char *buffer, size_t length) {
  int recv_size = recv(connection->socket, buffer, length, 0);
  if(recv_size <  0) return NetConnectionResult_Error;
  if(recv_size == 0) return NetConnectionResult_Disconnect;
  return recv_size;
}

NetConnectionSendResult os_net_send_sync(NetConnection *connection, char *buffer, size_t length) {
  int write_size = send(connection->socket, buffer, length, 0);
  if(write_size < 0) return NetConnectionResult_Error;
  return write_size;
}

