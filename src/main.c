#include <stdio.h>

#include "os/net.h"
#include "os/net.c"

#define PORT "8000"
#define DEFAULT_BUFLEN 512

int main(void) {
  printf("hello world\n");

  // @TODO: move this back into somewhere the os windows will use.
  NetInitResult response = os_win32_net_init();
  if(response != NetInitResult_Ok && response != NetInitResult_AlreadyInitialized) {
    // error here.
    printf("`os_win32_net_init` failed with: %d\n", response);
    return -1;
  }

  // server code.
  struct addrinfo *result = NULL;
  struct addrinfo *ptr    = NULL;
  struct addrinfo  hints  = {0};

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family   = AF_INET;      // using ipv4
  hints.ai_socktype = SOCK_STREAM;  // specify stream socket
  hints.ai_protocol = IPPROTO_TCP;  // TCP protocol
  hints.ai_flags    = AI_PASSIVE;   //

  // Resolve the local address and port to be used by the server
  int call_result = getaddrinfo(NULL, PORT, &hints, &result);
  if(call_result != 0) {
    printf("getaddrinfo failed: %d\n", call_result);
    os_win32_net_exit();
    return 1;
  }

  SOCKET listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if(listen_socket == INVALID_SOCKET) {
    printf("Error at socket(): %d\n", WSAGetLastError());
    freeaddrinfo(result);
    os_win32_net_exit();
    return 1;
  }

  // Setup the TCP listening socket
  call_result = bind(listen_socket, result->ai_addr, (int)result->ai_addrlen);
  if(call_result == SOCKET_ERROR) {
    printf("bind failed with error: %d\n", WSAGetLastError());
    freeaddrinfo(result);
    closesocket(listen_socket);
    os_win32_net_exit();
    return 1;
  }

  // we don't need this anymore once it is binded.
  freeaddrinfo(result);

  while(listen(listen_socket, SOMAXCONN) != SOCKET_ERROR) {
    // accept a connection here.
    SOCKET client_socket = accept(listen_socket, NULL, NULL);
    if(client_socket == INVALID_SOCKET) {
      continue;
    }


    char recv_buffer[DEFAULT_BUFLEN];
    int recv_buflen = DEFAULT_BUFLEN - 1;

    int recv_result = 0;
    for(;;) {
      recv_result = recv(client_socket, recv_buffer, recv_buflen, 0);
      if(recv_result < 0) {
        printf("`recv` failed with %d\n", WSAGetLastError());
        break;
      }

      if(recv_result == 0) {
        break;
      }

      recv_buffer[recv_result] = '\0';
      printf("Bytes received: %d\n", recv_result);
      printf("Received bytes: %s\n", recv_buffer);

      /*
      // Echo the buffer back to the sender
      int send_result = send(client_socket, recv_buffer, recv_result, 0);
      if (send_result == SOCKET_ERROR) {
      printf("send failed: %d\n", WSAGetLastError());
      closesocket(client_socket);
      WSACleanup();
      return 1;
      }
      printf("Bytes sent: %d\n", send_result);
      */
    }
    printf("Connection closing\n");
    closesocket(client_socket);
    break;
  }




  closesocket(listen_socket);
  os_win32_net_exit();
  return 0;
}
