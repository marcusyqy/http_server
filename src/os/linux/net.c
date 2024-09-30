#include "net.h"
#include "os/common.h"
#include <assert.h>

NetInitResult os_net_init(void) {
  /// asfaik it doesn't need this?
  /// SO it will always be ok
  return NetInitResult_Ok;
}

void os_net_exit(void) {
  /// does nothing
}

Os_NetConnection *os_linux_net_start_connection(const char *address, int port) {
  int domain = AF_INET;
  int type   = SOCK_STREAM;
  int sockfd = socket(domain, type, 0);

  // @TODO: address here?
  struct sockaddr_in server_addr = {0};
  server_addr.sin_family      = domain;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port        = htons(port);

  if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    return NULL;
  }

  Os_NetConnection *connection = malloc(sizeof(Os_NetConnection));
  connection->socket = sockfd;
  connection->meta   = server_addr;
  return connection;
}

void os_net_end_connection(Os_NetConnection *connection) {
  assert(connection);
  close(connection->socket);
  free(connection);
}

void os_listen(Os_NetConnection *connection, size_t max_connections) {

}

Os_NetConnection *os_linux_accept(Os_NetConnection *connection) {
  return NULL;
}

