#ifndef _OS_LINUX_NET_H_
#define _OS_LINUX_NET_H_

#include "os/common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct {
  int socket;
  struct sockaddr_in meta;
} NetConnection;

#endif //  _OS_LINUX_NET_H_
