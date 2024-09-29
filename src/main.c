#include <stdio.h>

#include "os/net.h"
#include "os/net.c"
#include "base.h"
#include "base.c"

#define PORT 8000
#define DEFAULT_BUFLEN 512

typedef struct {
  int x;
  int y;
  int z;
} Event;

typedef struct {
  Event *items;
  size_t count;
  size_t capacity;
} Events;

#if 0
int main(int arg_count, char **args) {
  Event *events = da_create(Event, 10);
  printf("cap: %zu\n", da_count(events));
  printf("cap: %zu\n", da_capacity(events));
  da_reserve(events, 100);

  Event event = {0};
  for(int i = 0; i < 100; ++i) {
    event.x = i;
    event.y = i;
    event.z = i;
    da_append(events, event);
    printf("%zu %zu\n", da_count(events), da_capacity(events));
  }

  printf("Verify\n");
  for(int i = 0; i < 100; ++i) {
    assert(events[i].x == i ||
           events[i].y == i ||
           events[i].z == i);
    printf("%d : Event { %d, %d, %d }\n", i, events[i].x, events[i].y, events[i].z);
  }

  da_free(events);
}

#endif

#if 0
int main(int arg_count, char **args) {
  (void)arg_count;
  (void)args;

  NetInitResult response = os_win32_net_init();
  if(response != NetInitResult_Ok && response != NetInitResult_AlreadyInitialized) {
    // error here.
    printf("`os_win32_net_init` failed with: %d\n", response);
    return -1;
  }
  // Resolve the local address and port to be used by the server
  OsWin32_NetConnection *connection = os_win32_net_start_connection(NULL, PORT, NULL);

  while(listen(connection->socket, SOMAXCONN) != SOCKET_ERROR) {
    // accept a connection here.
    SOCKET client_socket = accept(connection->socket, NULL, NULL);
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
    int end_result = shutdown(client_socket, SD_SEND);
    if(end_result == SOCKET_ERROR) {
      printf("shutdown failed: %d\n", WSAGetLastError());
    }
    closesocket(client_socket);
    break;
  }

  os_win32_net_end_connection(connection);
  os_win32_net_exit();
  return 0;
}
#endif


void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
  os_linux_net_init();
  OsLinux_NetConnection *connection = os_linux_net_start_connection(NULL, PORT, NULL);
  int newsockfd;
  socklen_t clilen;
  char buffer[256];
  int n;
  struct sockaddr_in cli_addr;

  listen(connection->socket, 5);
  clilen = sizeof(cli_addr);
  newsockfd = accept(connection->socket, (struct sockaddr *) &cli_addr, &clilen);
  if (newsockfd < 0)
    error("ERROR on accept");

  bzero(buffer,256);
  n = read(newsockfd,buffer,255);
  if (n < 0) error("ERROR reading from socket");
  printf("Here is the message: %s\n",buffer);
  n = write(newsockfd,"I got your message",18);
  if (n < 0) error("ERROR writing to socket");
  close(newsockfd);

  os_linux_net_end_connection(connection);
  os_linux_net_exit();
  return 0;
}
