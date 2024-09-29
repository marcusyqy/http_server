#include "net.h"
#include "os/common.h"
#include <assert.h>

NetInitResult os_linux_net_init(void) {
  /// asfaik it doesn't need this?
  /// SO it will always be ok
  return NetInitResult_Ok;
}

void os_linux_net_exit(void) {
  /// does nothing
}

OsLinux_NetConnection *os_linux_net_start_connection(const char *address, int port, NetConnectionResult *result) {

  int domain =  AF_INET;
  int type = SOCK_STREAM;
  int sockfd = sockfd = socket(domain, type, 0);

  // int newsockfd = 0;
  // struct sockaddr_in client_addr;

  struct sockaddr_in server_addr = {0};
  server_addr.sin_family = domain;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    return NULL;
  }

  OsLinux_NetConnection *connection = malloc(sizeof(OsLinux_NetConnection));
  connection->socket = sockfd;
  return connection;
}

void os_linux_net_end_connection(OsLinux_NetConnection *connection) {
  assert(connection);
  close(connection->socket);
  free(connection);
}

