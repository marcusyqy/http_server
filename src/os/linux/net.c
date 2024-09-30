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

NetConnection *os_net_start_connection(const char *address, int port) {
  int domain = AF_INET;
  int type   = SOCK_STREAM;
  int sockfd = socket(domain, type, 0);
  if(sockfd < 0) return NULL;

  // @TODO: address here?
  struct sockaddr_in server_addr = {0};
  server_addr.sin_family      = domain;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port        = htons(port);

  if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) return NULL;

  NetConnection *connection = malloc(sizeof(NetConnection));
  connection->socket = sockfd;
  connection->meta   = server_addr;
  return connection;
}

void os_net_end_connection(NetConnection *connection) {
  assert(connection);
  shutdown(connection->socket, SHUT_RDWR);
  close(connection->socket);
  free(connection);
}

void os_net_listen(NetConnection *connection, size_t max_connections) {
  listen(connection->socket, max_connections ? max_connections : SOMAXCONN);
}

NetConnection *os_net_accept(NetConnection *connection) {
  struct sockaddr_in client_addr = {0};
  socklen_t clilen = sizeof(client_addr);
  int new_sockfd = accept(connection->socket, (struct sockaddr *)&client_addr, &clilen);
  if(new_sockfd < 0) return NULL;

  NetConnection *new_connection = malloc(sizeof(NetConnection));
  new_connection->socket = new_sockfd;
  new_connection->meta   = client_addr;
  return new_connection;
}

NetConnectionRecvResult os_net_recv_sync(NetConnection *connection, char *buffer, size_t length) {
  int read_size = read(connection->socket, buffer, length);

  if(read_size == 0) return NetConnectionResult_Disconnect;
  if(read_size <  0) return NetConnectionResult_Error;

  return read_size;
}

NetConnectionSendResult os_net_send_sync(NetConnection *connection, char *buffer, size_t length) {
    int write_size = send(connection->socket, buffer, length, 0);
    if(write_size < 0) return NetConnectionResult_Error;
    return write_size;
}

