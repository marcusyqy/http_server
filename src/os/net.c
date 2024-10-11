#include "net.h"
#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
#pragma comment(lib, "Ws2_32.lib")
#elif defined(__linux__)
#include <errno.h>
#endif

NetInitResult os_net_init(void) {
#if defined(_WIN32)
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
#endif
  /// asfaik it doesn't need this?
  /// SO it will always be ok
  return NetInitResult_Ok;
}

void os_net_exit(void) {
#if defined(_WIN32)
  WSACleanup();
#endif
}

NetConnection *os_net_start_connection(const char *address, int port) {
#if defined(_WIN32)
  struct addrinfo *addr_result = NULL;
  struct addrinfo *addr_ptr    = NULL;
  struct addrinfo  addr_hints  = {0};

  ZeroMemory(&addr_hints, sizeof(addr_hints));
  addr_hints.ai_family   = AF_INET;      // using ipv4
  addr_hints.ai_socktype = SOCK_STREAM;  // specify stream socket
  addr_hints.ai_protocol = IPPROTO_TCP;  // TCP protocol
  addr_hints.ai_flags    = AI_PASSIVE;   //

  /// @TODO: remove all this error code handling from here?
  /// Resolve the local address and port to be used by the server
  char port_buffer[10] = {0};
  sprintf(port_buffer, "%d", port);
  int call_result = getaddrinfo(address, port_buffer, &addr_hints, &addr_result);
  if(call_result != 0) {
    fprintf(stderr, "getaddrinfo failed: %d\n", call_result);
    return NULL;
  }

  SOCKET listen_socket = socket(addr_result->ai_family, addr_result->ai_socktype, addr_result->ai_protocol);
  if(listen_socket == INVALID_SOCKET) {
    fprintf(stderr, "Error at socket(): %d\n", WSAGetLastError());
    freeaddrinfo(addr_result);
    return NULL;
  }

  call_result = bind(listen_socket, addr_result->ai_addr, (int)addr_result->ai_addrlen);
  if(call_result == SOCKET_ERROR) {
    fprintf(stderr, "bind failed with error: %d\n", WSAGetLastError());
    freeaddrinfo(addr_result);
    closesocket(listen_socket);
    return NULL;
  }
  freeaddrinfo(addr_result);

#elif defined(__linux__)
  int domain = AF_INET;
  int type   = SOCK_STREAM;
  int listen_socket = socket(domain, type, 0);
  if(listen_socket < 0) return NULL;

  // @TODO: address here?
  struct sockaddr_in server_addr = {0};
  server_addr.sin_family      = domain;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port        = htons(port);

  if(bind(listen_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) return NULL;
#else
#error not supported
#endif

  NetConnection *connection = malloc(sizeof(NetConnection));
  connection->socket = listen_socket;
  connection->type = NetSocketType_None;
  return connection;
}

void os_net_end_connection(NetConnection connection[static 1]) {
  if((connection->type & NetSocketType_SendRecv) != 0) {
#if defined(_WIN32)
    int end_result = shutdown(connection->socket, SD_SEND);
    if(end_result == -1) {
      fprintf(stderr, "shutdown failed: %d\n", WSAGetLastError());
    }
#elif defined(__linux__)
    int end_result = shutdown(connection->socket, SHUT_WR);
    if(end_result == -1) {
      fprintf(stderr, "shutdown failed: %d\n", errno);
    }
#else
#error platform not supported!
#endif
  }

#if defined(_WIN32)
  closesocket(connection->socket);
#elif defined(__linux__)
  close(connection->socket);
#else
#error platform not supported!
#endif
  free(connection);
}

void os_net_listen(NetConnection connection[static 1], size_t max_connections) {
  /// @TODO: we have to change this.
  /// usually we while this but here we need to do something smarter.
  if(listen(connection->socket, max_connections ? max_connections : SOMAXCONN) == -1) {
    fprintf(stdout, "%s", "Something went wrong during os_net_listen");
  }
}

NetConnection *os_net_accept(NetConnection connection[static 1]) {
  NetSocket client_socket = accept(connection->socket, NULL, NULL);
  if(client_socket < 0) return NULL;

  NetConnection *new_connection = malloc(sizeof(NetConnection));
  new_connection->socket = client_socket;
  new_connection->type = NetSocketType_SendRecv;
  return new_connection;
}

NetConnectionRecvResult os_net_recv_sync(NetConnection connection[static 1], char buffer[static 1], size_t length) {
  int recv_size = recv(connection->socket, buffer, length, 0);
  if(recv_size <  0) return NetConnectionResult_Error;
  if(recv_size == 0) return NetConnectionResult_Disconnect;
  return recv_size;
}

NetConnectionSendResult os_net_send_sync(NetConnection connection[static 1], char buffer[static 1], size_t length) {
  int write_size = send(connection->socket, buffer, length, 0);
  if(write_size < 0) return NetConnectionResult_Error;
  return write_size;
}


